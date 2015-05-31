#include "FragmentMgr.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include "../../util/Uberlogger.h"

#define DELIMITER 5

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

void FragmentMgr::changeLoad(uint64_t newLoad) {
  _lastweight = newLoad;
  ULOG(load) << newLoad << UEND;
}

void FragmentMgr::addFragment(Fragment* fragment) {
  bool tick = true;
  if(fragment->isVirtual()) {
     Fragment* master = 0;
     for(auto &f : fragments) {
       if(f.first->id().vcompare(fragment->id()))
           master = f.first;
     }

     if(master != 0) {
         master->merge(fragment);
     }
     tick = false;
  }

  auto b = findExternalFragment(fragment->id());

  if(b != externalFragments.end()) {
    (*b)->moveStates(fragment);
    externalFragments.erase(b);
    tick = false;
  }

  fragments[fragment] = FREE;
  if(tick) normalFragmentsCounter++;
  _weight += fragment->weight();
  uint64_t peka = (_weight >= _lastweight) ? (_weight - _lastweight) : (_lastweight - _weight);
  if(peka >= _lastweight / DELIMITER) {
    for(auto& n: neighbours) messageMgr->sendLoad(n, _weight);
    changeLoad(_weight);
    system->balancerNotify();
  }

  system->notify();
}

typedef pair<Fragment*, vector<Fragment*> > WorkFragment;

vector<WorkFragment> FragmentMgr::getFragments(int size) {
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

  for(auto fragment: ffragments) {
    /// Check fragment waiting for final reduce step
    if(!fragment->needReduce() && !fragment->wasReduced()) {
      ++reduceCount;
      reduceResult.push_back(WorkFragment(fragment, vector<Fragment*>()));
      continue;
    }

    if(!fragment->needNeighbours() && !fragment->needVNeighbours()) {
      result.push_back(WorkFragment(fragment, vector<Fragment*>()));
      continue;
    }

    vector<ID> neighboursID;
    if(fragment->needNeighbours()) {
        neighboursID = fragment->neighbours();
    } else if(fragment->needVNeighbours()) {
        neighboursID = fragment->vneighbours();
    }

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

  if(reduceCount == normalFragmentsCounter) {
    for(auto i: reduceResult) {
      fragments[i.first] = EXEC;
    }

    //fragmentsLock.unlock();
    //external//fragmentsLock.unlock();
    return reduceResult;
  }
  else {
    int count = 0;
    vector<WorkFragment> r;
    for(auto i: result) {
      fragments[i.first] = EXEC;
      r.push_back(i);
      if(++count == size) {
        break;
      }
    }
    return r;
  }
}

void FragmentMgr::unlock(Fragment* fragment) {
  if(fragment->needUpdate()) {
    auto nodes = fragment->noticeList();
    auto lastState = fragment->getLastState();
    for(auto node: nodes) messageMgr->sendBoundary(node, lastState);
  }
  if(fragment->needVUpdate()) {
    auto nodes = fragment->vnoticeList();
    auto lastState = fragment->getLastState();
    for(auto node: nodes) messageMgr->sendBoundary(node, lastState);
  }

  if(fragments[fragment] == EXEC)
     fragments[fragment] = FREE;

  if(fragment->isEnd()) {
    if(fragment->isHalt()) {
      system->end();
      messageMgr->sendHalt();
      return;
    }
    _weight -= fragment->weight();
    uint64_t peka = (_weight >= _lastweight) ? (_weight - _lastweight) : (_lastweight - _weight);
    if(peka >= _lastweight / DELIMITER) {
      for(auto& n: neighbours) messageMgr->sendLoad(n, _weight);
      changeLoad(_weight);
      system->balancerNotify();
    }
  }
}

void FragmentMgr::updateExternalFragment(Fragment* fragment) {
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
  for(auto& state: fragment->_vstates) {
    messageMgr->sendBoundary(moveList.at(id), state.second);
  }
  /// Send boundaries to node
  specialUpdateNeighbours(fragment);

  /// Remove Fragment from list
  //fragmentsLock.wlock();
  fragments.erase(fragment);
  normalFragmentsCounter--;
  //fragmentsLock.unlock();
  createExternal(fragment);

  auto movefragmentit = find_if(moveList.begin(), moveList.end(),
                                [&id](pair<ID, NodeID> f) {
                                  return f.first == id;
                                });
  if(movefragmentit == moveList.end()) {
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
  //fragmentsLock.wlock();
  fragments[fragment] = MOVE;
  _weight -= fragment->weight();
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
    auto n = findFragment(neighbour);
    if(n != 0) {
      //XXX.
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

uint64_t FragmentMgr::weight() {
  return _weight;
}

void FragmentMgr::moveFragment(const std::map<NodeID, double>& amount) {
  int currentWeight = weight();
  for(auto& a : amount) {
    NodeID node = a.first;
    double absolute = a.second * currentWeight;

    for(auto& f : fragments) {
      if(f.second == FREE && !f.first->isEnd() && f.first->isMovable()) {
        startMoveFragment(f.first, node);
        absolute -= f.first->weight();
        if(absolute <= 0) break;
      }
    }
  }
  uint64_t peka = (_weight >= _lastweight) ? (_weight - _lastweight) : (_lastweight - _weight);
  if(peka >= _lastweight / DELIMITER) {
    for(auto& n: neighbours) messageMgr->sendLoad(n, _weight);
    changeLoad(_weight);
  }
}
}}
