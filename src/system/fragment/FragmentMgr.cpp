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

FragmentMgr::FragmentMgr():
  fragmentTools(0),
  messageMgr(0),
  system(0) {}

FragmentMgr::FragmentMgr(MessageMgr* msgMgr):
  FragmentMgr() {
  messageMgr = msgMgr;
}
FragmentMgr::~FragmentMgr() {}

void FragmentMgr::addFragment(Fragment* fragment) {
  //external//fragmentsLock.wlock();
  auto b = findExternalFragment(fragment->id());

  if(b != externalFragments.end()) {
    (*b)->moveStates(fragment);

    //external//fragmentsLock.wlock();
    externalFragments.erase(b);
    //external//fragmentsLock.unlock();
  }
  //external//fragmentsLock.unlock();

  //fragmentsLock.wlock();
  fragments[fragment] = FREE;
  //fragmentsLock.unlock();
  system->notify();
}

typedef pair<Fragment*, vector<Fragment*> > WorkFragment;

vector<WorkFragment> FragmentMgr::getFragments(int) {
  //external//fragmentsLock.rlock();
  //fragmentsLock.wlock();

  vector<WorkFragment> result;
  vector<WorkFragment> reduceResult;
  size_t reduceCount = 0;
  size_t endCount = 0;

  /// Find all non-blocked fragments without end state.
  vector<Fragment*> ffragments;

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
//  if(endCount == fragments.size()) {
//    //system->end();
//    //fragmentsLock.unlock();
//    return vector<WorkFragment>();
//  }

  if(ffragments.size() == 0) {
    ULOG(default) << "There is nothing. Ok?" << UEND;
  }
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
                                  return *(fragment.first) == i && (fragment.second == FREE || fragment.second == EXEC);
                                });

      if (fragments.end() == fragmentit) {
        /// If neighbour fragment doesn't exist in local fragments
        /// try to find it in external fragments

        auto fragmentit = find_if(externalFragments.begin(),
                                  externalFragments.end(),
                                  [&i](Fragment* fragment) {
          return *fragment == i;
        });

        if (externalFragments.end() == fragmentit) {
          break;
        }
        else
          findedFragment = *fragmentit;
      } else {
        findedFragment = fragmentit->first;
      }

      if(!findedFragment->hasState(fragment->neighboursState())) {
        /// It means that finded copy of external fragment has
        /// too old state
        ULOG(fragment) << "OLD STATE: " << fragment->id().tostr() <<
                            " need state: " <<
                            std::get<0>(fragment->neighboursState()) <<
                            ":" <<
                            std::get<1>(fragment->neighboursState()) <<
                            " from " << findedFragment->id().tostr() <<
                            UEND;
        break;
      }
      fwithstates.push_back(findedFragment);
    }

    if(fwithstates.size() == neighboursID.size()) {
        for(auto &s : fwithstates) {
          neighbours.push_back(s->getState(fragment->neighboursState(),
                                           fragment->id()));
        }
        result.push_back(pair<Fragment*, vector<Fragment*>>(fragment,
                                                            neighbours));
    }
  }

  if(reduceCount == fragments.size()) {
    for(auto i: reduceResult) {
      ULOG(move) << "Pushed back for execution fragment " << i.first->id().tostr() << UEND;
      fragments[i.first] = EXEC;
    }

    //fragmentsLock.unlock();
    //external//fragmentsLock.unlock();
    return reduceResult;
  }
  else {
    for(auto i: result) {
      ULOG(move) << "Pushed back for execution fragment " << i.first->id().tostr() << UEND;
      fragments[i.first] = EXEC;
    }
    //fragmentsLock.unlock();
    //external//fragmentsLock.unlock();
    return result;
  }
}

void FragmentMgr::unlock(Fragment* fragment) {
  //fragmentsLock.rlock();
  if(fragment->needUpdate()) {
    auto nodes = fragment->noticeList();
    auto lastState = fragment->getLastState();
    for(auto node: nodes) messageMgr->sendBoundary(node, lastState);
  }
  //fragmentsLock.unlock();

  //fragmentsLock.wlock();
  if(fragments[fragment] == EXEC)
     fragments[fragment] = FREE;
  //fragmentsLock.unlock();

  if(system->id() == 0)
    startMoveFragment(fragment, 1);
  if(system->id() == 1)
    startMoveFragment(fragment, 0);
}

void FragmentMgr::updateExternalFragment(Fragment* fragment) {
  ULOG(fragment) << "NEW EXTERNAL CELL STATE: " <<
                      fragment->id().tostr() << " with stamp: " <<
                      fragment->iteration() << ":" << fragment->progress() <<
                      UEND;

  ID id = fragment->id();
  auto timestamp = std::tuple<uint64_t, uint64_t>(fragment->iteration(), fragment->progress());
  // Find external fragment
  auto efr = findExternalFragment(id);
  //external//fragmentsLock.rlock();
  bool notFinded = (efr == externalFragments.end());
  //external//fragmentsLock.unlock();
  if(!notFinded) {
    //external//fragmentsLock.wlock();
    if(!(*efr)->hasState(timestamp)) {
      (*efr)->saveState(fragment);
    }
    //external//fragmentsLock.unlock();
    system->notify();
    return;
  }

  // Find internal fragment
  auto ifr = findInternalFragment(id);
  //fragmentsLock.rlock();
  notFinded = (ifr == fragments.end());
  //fragmentsLock.unlock();
  if(!notFinded) {
    //fragmentsLock.wlock();
    if(!ifr->first->hasState(timestamp)) {
      ifr->first->saveState(fragment);
    }
    //fragmentsLock.unlock();
    system->notify();
    return;
  }

  // Fragment wasn't finded

  ID fragmentID = id;
  Fragment* newFragment = fragmentTools->createGap(fragmentID);
  newFragment->setBoundary();


  for(auto &i : fragment->_vneighboursLocation) {
    newFragment->addNeighbour(i.first, i.second); //XXX !!!
  }

  newFragment->setNodeID(system->id());
  newFragment->saveState(fragment);

  //external//fragmentsLock.wlock();
  externalFragments.push_back(newFragment);
  //external//fragmentsLock.unlock();

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
  messageMgr->sendFullFragment(moveList.at(id), fragment);
  /// Send boundaries to node
  specialUpdateNeighbours(fragment);

  /// Remove Fragment from list
  ULOG(move) << "Fragment " << fragment->id().tostr() << " was moved" << UEND;
  //fragmentsLock.wlock();
  fragments.erase(fragment);
  //fragmentsLock.unlock();
  createExternal(fragment);

  auto movefragmentit = find_if(moveList.begin(), moveList.end(),
                                [&id](pair<ID, NodeID> f) {
                                  return f.first == id;
                                });
  if(movefragmentit == moveList.end()) {
    ULOG(default) << "That is not cool" << UEND;
    exit(2);
  }
  moveList.erase(movefragmentit);

  auto movefragmentacceptit = find_if(movingFragmentAccept.begin(),
                                        movingFragmentAccept.end(),
                                        [&id](pair<ID, vector<NodeID> > f) {
                                          return f.first == id;
                                        });
  if(movefragmentacceptit != movingFragmentAccept.end())
    movingFragmentAccept.erase(movefragmentacceptit);

  delete fragment;
}

void FragmentMgr::startMoveFragment(Fragment* fragment, NodeID node) {
  ULOG(move) << "I'm starting moving fragment " << fragment->id().tostr() << " to " << node << UEND;
  //fragmentsLock.wlock();
  fragments[fragment] = MOVE;
  //fragmentsLock.unlock();
  moveList[fragment->id()] = node;
  messageMgr->sendStartMove(node, fragment->id(), node); // XXX: Second node not needed.
}

void FragmentMgr::createExternal(Fragment* f) {
  Fragment* nf = fragmentTools->createGap(f->id());
  nf->setBoundary();
  f->createExternal(nf);
  externalFragments.push_back(nf);
}

void FragmentMgr::noticeMoveFragment(const ID& id) {
  //fragmentsLock.rlock();
  Fragment* fragment = findInternalFragment(id)->first;
  //fragmentsLock.unlock();

  //fragmentsLock.rlock();
  auto neighbours = fragment->noticeList();
  neighbours.erase(moveList[id]);
  //fragmentsLock.unlock();

  if(neighbours.size() == 0) {
    moveFragment(findInternalFragment(id)->first);
    return;
  }

  movingFragmentAccept.emplace(id, vector<NodeID>());

  for(auto& i : neighbours) {
    if(i != moveList[id])
      movingFragmentAccept.at(id).push_back(i);
  }

  for(auto i: neighbours) {
    if(i != moveList[id])
      messageMgr->sendNoticeMove(i, id, moveList[id]);
  }
}

void FragmentMgr::specialUpdateNeighbours(ts::type::Fragment* fragment) {
  auto neighbours = fragment->neighbours();
  auto id = fragment->id();

  for(auto neighbour : neighbours) {
    ULOG(error) << "Try to find " << id.tostr() << " neighbout with id " << neighbour.tostr() << UEND;
    auto n = findFragment(neighbour);
    if(n != 0) {
      ULOG(error) << "Neighbour with id " << neighbour.tostr() << " was finded" << UEND;
      //XXX.
      auto states = n->specialUpdateNeighbour(id, moveList[id]);
      for(auto s : states) {
        messageMgr->sendBoundary(moveList[id], s);
        delete s;
      }
    } else {
      ULOG(error) << "Neighbour with id " << neighbour.tostr() << " was not finded" << UEND;
    }
  }
}

void FragmentMgr::confirmMove(const ts::type::ID& id, NodeID node) {
  messageMgr->sendConfirmMove(node, id);
}

void FragmentMgr::globalConfirmMove(const ts::type::ID& id, NodeID node) {
  messageMgr->sendGlobalConfirmMove(node, id);
}

void FragmentMgr::updateNeighbours(const ts::type::ID& id, NodeID node) {
  ////fragmentsLock.wlock();
  for(auto i : fragments)
    if(i.first->isNeighbour(id))
      i.first->updateNeighbour(id, node);
  ////fragmentsLock.unlock();

  ////external//fragmentsLock.wlock();
  for(auto i : externalFragments)
    if(i->isNeighbour(id))
      i->updateNeighbour(id, node);
  ////external//fragmentsLock.unlock();
}

void FragmentMgr::moveFragmentAccept(const ts::type::ID& id) {
  noticeMoveFragment(id);
}

void FragmentMgr::moveFragmentGlobalAccept(const ts::type::ID& id, NodeID nid) {
  auto it = std::find(movingFragmentAccept[id].begin(),
                      movingFragmentAccept[id].end(), nid);
  if(it == movingFragmentAccept[id].end()) {
    ULOG(move) << "Some node send accept without request for id: " << id.tostr() << UEND;
    return;
  }

  movingFragmentAccept[id].erase(it);
  if(movingFragmentAccept[id].empty()) {
    moveFragment(findInternalFragment(id)->first);
  }
}

Fragment* FragmentMgr::findFragment(const ts::type::ID& id) {
  auto fragmentit = findInternalFragment(id);
  if(fragmentit == fragments.end()) {
    auto efragmentit = findExternalFragment(id);
    if(efragmentit == externalFragments.end()) {
      return 0;
    }
    else
      return *efragmentit;
  }
  else
    return fragmentit->first;
}

FragmentMgr::internalit
FragmentMgr::findInternalFragment(const ts::type::ID& id) {
  //external//fragmentsLock.rlock();
  auto it = find_if(fragments.begin(), fragments.end(),
                    [&id](pair<Fragment*, bool> f) {
                      return *f.first == id;
                    });
  //external//fragmentsLock.unlock();
  return it;
}

FragmentMgr::externalit
FragmentMgr::findExternalFragment(const ts::type::ID& id) {
  //external//fragmentsLock.rlock();
  auto it = find_if(externalFragments.begin(), externalFragments.end(),
                    [&id](Fragment* f) {
                      return *f == id;
                    });
  //external//fragmentsLock.unlock();
  return it;
}
}}
