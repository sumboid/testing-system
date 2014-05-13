#include "FragmentMgr.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include "../../util/Uberlogger.h"

using std::pair;
using std::vector;
using std::map;
using ts::type::Fragment;
using ts::type::ID;

namespace ts {
namespace system {

FragmentMgr::FragmentMgr() {}
FragmentMgr::FragmentMgr(MessageMgr* msgMgr): messageMgr(msgMgr) {}
FragmentMgr::~FragmentMgr() {}

void FragmentMgr::addFragment(Fragment* fragment) {
  Fragment* b = findFragment(fragment->id());

  if(b != 0) {
    b->moveStates(fragment);
  }

  fragmentsLock.wlock();
  fragments[fragment] = FREE;
  fragmentsLock.unlock();
  system->notify();
}

typedef pair<Fragment*, vector<Fragment*> > WorkFragment;

vector<WorkFragment> FragmentMgr::getFragments(int) {
  vector<WorkFragment> result;
  vector<WorkFragment> reduceResult;
  size_t reduceCount = 0;
  size_t endCount = 0;

  /// Find all non-blocked fragments without end state.
  vector<Fragment*> ffragments;

  fragmentsLock.rlock();
  for(auto fragment: fragments)
    if(fragment.second == FREE) {
      if(!fragment.first->isEnd()) {
        ffragments.push_back(fragment.first);
      }
      else {
        ++endCount;
      }
    }

  /// If all of fragments are free with end state shut down the System
  if(endCount == fragments.size()) {
    system->end();
    fragmentsLock.unlock();
    return vector<WorkFragment>();
  }

  fragmentsLock.unlock();

  for(auto fragment: ffragments) {
    /// Check fragment waiting for final reduce step
    if(!fragment->needReduce() && !fragment->wasReduced()) {
      ++reduceCount;
      reduceResult.push_back(WorkFragment(fragment, vector<Fragment*>()));
      break;
    }

    if(!fragment->needNeighbours()) {
      result.push_back(WorkFragment(fragment, vector<Fragment*>()));
      break;
    }

    vector<ID> neighboursID = fragment->neighbours();
    vector<Fragment*> fwithstates;
    vector<Fragment*> neighbours;

    /// Find all neighbours of current fragment
    for(auto i: neighboursID) {
      Fragment* findedFragment;

      /// Find neighbour fragment in local fragments
      auto fragmentit = find_if(fragments.begin(), fragments.end(),
                           [&i](pair<Fragment*, bool> fragment){
        return *fragment.first == i;
      });

      if (fragments.end() == fragmentit) {
        /// If neighbour fragment doesn't exist in local fragments
        /// try to find it in external fragments

        externalFragmentsLock.rlock();
        auto fragmentit = find_if(externalFragments.begin(),
                                  externalFragments.end(),
                                  [&i](Fragment* fragment) {
          return *fragment == i;
        });

        if (externalFragments.end() == fragmentit) {
          externalFragmentsLock.unlock();
          break;
        }
        else
          findedFragment = *fragmentit;
        externalFragmentsLock.unlock();
      } else {
        findedFragment = fragmentit->first;
      }

      if(!findedFragment->hasState(fragment->neighboursState())) { // Need to change
        /// It means that finded copy of external fragment has
        /// too old state
        UBERLOG("fragment") << "OLD STATE: " << fragment->id().tostr() << " need state: " << std::get<0>(fragment->neighboursState()) << ":" << std::get<1>(fragment->neighboursState()) << " from " << findedFragment->id().tostr() << UBEREND();
        break;
      }
      fwithstates.push_back(findedFragment);
    }

    if(fwithstates.size() == neighboursID.size()) {
        for(auto &s : fwithstates) {
          neighbours.push_back(s->getState(fragment->neighboursState(), fragment->id())); // !!!!!!!!!!
        }
        result.push_back(pair<Fragment*, vector<Fragment*>>(fragment, neighbours));
    }
  }

  fragmentsLock.wlock();
  if(reduceCount == fragments.size()) {
    for(auto i: reduceResult)
      fragments[i.first] = EXEC;
    fragmentsLock.unlock();

    return reduceResult;
  }
  else {
    for(auto i: result)
      fragments[i.first] = EXEC;
    fragmentsLock.unlock();
    return result;
  }
}

void FragmentMgr::unlock(Fragment* fragment) {
  if(fragment->needUpdate()) {
    auto nodes = fragment->noticeList();
    for(auto node: nodes) messageMgr->sendBoundary(node, fragment->getLastState());
  }

  fragmentsLock.wlock();
  if(fragments[fragment] == EXEC)
    fragments[fragment] = FREE;
  fragmentsLock.unlock();

  if(system->id() == 0)
    startMoveFragment(fragment, 1);
  if(system->id() == 1)
    startMoveFragment(fragment, 0);
}

void FragmentMgr::updateExternalFragment(Fragment* fragment) {
  UBERLOG("fragment") << "NEW EXTERNAL CELL STATE: " << fragment->id().tostr() <<
    " with stamp: " << fragment->iteration() << ":" << fragment->progress() << UBEREND();

  externalFragmentsLock.rlock();

  for(auto ffragment: externalFragments) {
    if(ffragment->id() == fragment->id()) {

      externalFragmentsLock.unlock();

      externalFragmentsLock.wlock();
      ffragment->saveState(fragment);
      externalFragmentsLock.unlock();

      system->notify();
      return;
    }
  }

  externalFragmentsLock.unlock();

  ID fragmentID = fragment->id();
  Fragment* newFragment = fragmentTools->createGap(fragmentID);

  for(auto &i : fragment->_vneighboursLocation) {
    newFragment->addNeighbour(i.first, i.second); //XXX !!!
  }

  newFragment->setNodeID(system->id());
  newFragment->saveState(fragment);

  externalFragmentsLock.wlock();
  externalFragments.push_back(newFragment);
  externalFragmentsLock.unlock();

  system->notify();
}

vector<Fragment*> FragmentMgr::getFragments() {
  vector<Fragment*> result;

  for(auto fragment : fragments) {
    result.push_back(fragment.first);
  }

  return result;
}

void FragmentMgr::moveFragment(Fragment* fragment) {
  ID id = fragment->id();

  /// Send Fragment to node
  messageMgr->sendFullFragment(moveList[id], fragment);

  /// Remove Fragment from list
  fragmentsLock.rlock();
  auto fragmentit = find_if(fragments.begin(), fragments.end(),
                          [&id](pair<Fragment*, bool> f) {
                            return *f.first == id;
                          });
  fragmentsLock.unlock();
  fragmentsLock.wlock();

  fragments.erase(fragmentit);

  fragmentsLock.unlock();

  auto movefragmentit = find_if(moveList.begin(), moveList.end(),
                              [&id](pair<ID, NodeID> f) {
                                return f.first == id;
                              });
  moveList.erase(movefragmentit);

  auto movefragmentacceptit = find_if(movingFragmentAccept.begin(), movingFragmentAccept.end(),
                                    [&id](pair<ID, vector<NodeID> > f) {
                                      return f.first == id;
                                   });
  movingFragmentAccept.erase(movefragmentacceptit);

  delete fragment;
}

void FragmentMgr::startMoveFragment(Fragment* fragment, NodeID node) {
  fragmentsLock.rlock();

  ID id = fragment->id();
  auto neighbours = fragment->noticeList();
  fragmentsLock.unlock();

  movingFragmentAccept.emplace(id, vector<NodeID>());

  for(auto i : neighbours) {
    movingFragmentAccept.at(id).push_back(i);
  }

  fragmentsLock.wlock();
  fragments[fragment] = MOVE;
  fragmentsLock.unlock();

  moveList[fragment->id()] = node;

  specialUpdateNeighbours(id);

  for(auto i: neighbours) messageMgr->sendStartMove(i, id, node);

  if(neighbours.empty()) {
    moveFragment(fragment);
  }
}

void FragmentMgr::createExternal(Fragment* f) {
  Fragment* nf = fragmentTools->createGap(f->id());
  f->createExternal(nf);
  externalFragments.push_back(nf);
}

void FragmentMgr::specialUpdateNeighbours(const ts::type::ID& id) {
  auto fragment = findFragment(id);
  auto neighbours = fragment->neighbours();

  for(auto neighbour : neighbours) {
    auto n = findFragment(neighbour);
    if(n != 0) {
      auto states = n->specialUpdateNeighbour(id, moveList[id]);
      for(auto s : states) {
        messageMgr->sendBoundary(moveList[id], s);
        delete s;
      }
    }
  }
}

void FragmentMgr::confirmMove(const ts::type::ID& id, NodeID node) {
  messageMgr->sendConfirmMove(node, id);
}

void FragmentMgr::updateNeighbours(const ts::type::ID& id, NodeID node) {
  for(auto i : fragments)
    if(i.first->isNeighbour(id))
      i.first->updateNeighbour(id, node);

  for(auto i : externalFragments)
    if(i->isNeighbour(id))
      i->updateNeighbour(id, node);
}

void FragmentMgr::moveFragmentAccept(const ts::type::ID& id, NodeID nid) {
  auto it = std::find(movingFragmentAccept[id].begin(), movingFragmentAccept[id].end(), nid);
  if(it == movingFragmentAccept[id].end()) {
    ULOG(move) << "Some node send accept without request" << UBEREND();
  }
  movingFragmentAccept[id].erase(it);
  if(movingFragmentAccept[id].empty()) {
    messageMgr->sendFullFragment(moveList[id], findFragment(id));
  }
}

Fragment* FragmentMgr::findFragment(const ts::type::ID& id) {
  externalFragmentsLock.rlock();
  auto fragmentit = find_if(fragments.begin(), fragments.end(),
                            [&id](pair<Fragment*, bool> f) {
                              return *f.first == id;
                            });
  externalFragmentsLock.unlock();
  if(fragmentit == fragments.end()) {
    externalFragmentsLock.rlock();
    auto efragmentit = find_if(externalFragments.begin(), externalFragments.end(),
                          [&id](Fragment* f) {
                            return *f == id;
                          });
    externalFragmentsLock.unlock();
    if(efragmentit == externalFragments.end()) {
      return 0;
    }
    else
      return *efragmentit;
  }
  else
    return fragmentit->first;
}

}}
