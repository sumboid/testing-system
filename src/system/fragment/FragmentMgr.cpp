#include "FragmentMgr.h"
#include <iostream>
#include <algorithm>

using std::pair;
using std::vector;
using std::map;
using ts::type::Fragment;
using ts::type::ID;

namespace ts {
namespace system {

FragmentMgr::FragmentMgr() {
  fragmentsLock = new pthread_rwlock_t;
  externalFragmentsLock = new pthread_rwlock_t;
  pthread_rwlock_init(fragmentsLock, NULL);
  pthread_rwlock_init(externalFragmentsLock, NULL);
}

FragmentMgr::FragmentMgr(MessageMgr* msgMgr):
  messageMgr(msgMgr) {
  fragmentsLock = new pthread_rwlock_t;
  pthread_rwlock_init(fragmentsLock, NULL);
}

FragmentMgr::~FragmentMgr() {
  pthread_rwlock_destroy(fragmentsLock);
  pthread_rwlock_destroy(externalFragmentsLock);
  delete fragmentsLock;
  delete externalFragmentsLock;
}

void FragmentMgr::addFragment(Fragment* fragment) {
  pthread_rwlock_wrlock(fragmentsLock);
  fragments[fragment] = FREE;
  pthread_rwlock_unlock(fragmentsLock);
}

typedef pair<Fragment*, vector<Fragment*> > WorkFragment;
vector<WorkFragment> FragmentMgr::getFragments(int amount) {
  vector<WorkFragment> result;
  vector<WorkFragment> reduceResult;
  size_t reduceCount = 0;
  size_t endCount = 0;

  /// Find all non-blocked fragments without end state.
  vector<Fragment*> ffragments;

  pthread_rwlock_rdlock(fragmentsLock);
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
    pthread_rwlock_unlock(fragmentsLock);
    return vector<WorkFragment>();
  }

  pthread_rwlock_unlock(fragmentsLock);

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

        pthread_rwlock_rdlock(externalFragmentsLock);
        auto fragmentit = find_if(externalFragments.begin(),
                              externalFragments.end(), 
                              [&i](Fragment* fragment) {
          return *fragment == i;
        });

        if (externalFragments.end() == fragmentit) {
          pthread_rwlock_unlock(externalFragmentsLock);
          break;
        }
        else
          findedFragment = *fragmentit;
        pthread_rwlock_unlock(externalFragmentsLock);
      } else {
        findedFragment = fragmentit->first;
      }

      if(!findedFragment->hasState(fragment->neighboursState())) { // Need to change
        /// It means that finded copy of external fragment has
        /// too old state
        break;
      }
      neighbours.push_back(findedFragment->getState(fragment->neighboursState(), fragment->id()));
    }

    if(neighbours.size() == neighboursID.size()) {
        result.push_back(pair<Fragment*, vector<Fragment*>>(fragment, neighbours));
    }
  }

  pthread_rwlock_wrlock(fragmentsLock);
  if(reduceCount == fragments.size()) {
    for(auto i: reduceResult)
      fragments[i.first] = EXEC;
    pthread_rwlock_unlock(fragmentsLock);

    return reduceResult;
  }
  else {
    for(auto i: result)
      fragments[i.first] = EXEC;
    pthread_rwlock_unlock(fragmentsLock);
    return result;
  }
}

void FragmentMgr::unlock(Fragment* fragment) {
  if(fragment->needUpdate()) {
    auto nodes = fragment->noticeList();
    for(auto node: nodes) messageMgr->sendBoundary(node, fragment->getLastState());
  }
  pthread_rwlock_wrlock(fragmentsLock);
  if(fragments[fragment] == EXEC)
    fragments[fragment] = FREE;
  pthread_rwlock_unlock(fragmentsLock);
}

void FragmentMgr::updateExternalFragment(Fragment* fragment) {
  std::cout << system->id() << ": NEW EXTERNAL CELL STATE: " << fragment->id().tostr() << 
    " with stamp: " << fragment->iteration() << ":" << fragment->progress() << std::endl;
  pthread_rwlock_rdlock(externalFragmentsLock);

  for(auto ffragment: externalFragments) {
    if(ffragment->id() == fragment->id()) {
      auto it = find(externalFragments.begin(), externalFragments.end(), ffragment);

      pthread_rwlock_unlock(externalFragmentsLock);
      pthread_rwlock_wrlock(externalFragmentsLock);

      (*it)->saveState(fragment);

      pthread_rwlock_unlock(externalFragmentsLock);
      system->notify();
      return;
    }
  }

  pthread_rwlock_unlock(externalFragmentsLock);
  pthread_rwlock_wrlock(externalFragmentsLock);

  ID fragmentID = fragment->id();
  Fragment* newFragment = fragmentTools->createGap(fragmentID);
  newFragment->saveState(fragment);
  externalFragments.push_back(newFragment);
  pthread_rwlock_unlock(externalFragmentsLock);
  system->notify();
}

vector<Fragment*> FragmentMgr::getFragments() {
  vector<Fragment*> result;

  for(auto fragment : fragments) {
    result.push_back(fragment.first);
  }

  return result;
}

void FragmentMgr::moveFragment(ts::type::Fragment* fragment, NodeID node) {
  pthread_rwlock_rdlock(fragmentsLock);
  ID id = fragment->id();
  auto neighbours = fragment->noticeList();
  pthread_rwlock_unlock(fragmentsLock);

  pthread_rwlock_wrlock(fragmentsLock);
  fragments[fragment] = MOVE;
  pthread_rwlock_unlock(fragmentsLock);

  moveList[fragment->id()] = node;

  for(auto i: neighbours) movingFragmentAccept[id].push_back(i);
  updateNeighbours(id);

  if(neighbours.empty()) {
    Fragment* f = findFragment(id);

    /// Send Fragment to node
    messageMgr->sendFullFragment(moveList[id], f);
    auto fragmentit = find_if(fragments.begin(), fragments.end(),
                            [&id](pair<Fragment*, bool> f) {
                              return *f.first == id;
                            });

    /// Remove Fragment from list
    fragments.erase(fragmentit);

    /// Send Boundaries of all that needed Fragment
    /// Clean up boundaries and external fragments
    
    delete f;
  }
  else {
    for(auto i: neighbours) messageMgr->sendStartMove(i, id, node);
  }
}

void FragmentMgr::createExternal(ts::type::Fragment* f) {
  Fragment* nf = fragmentTools->createGap(f->id());
  f->createExternal(nf);
  externalFragments.push_back(nf);
}

void FragmentMgr::updateNeighbours(const ts::type::ID& id) {
  auto fragment = findFragment(id);
  auto neighbours = fragment->neighbours(system->id());

  for(auto neighbour : neighbours) {
    auto n = findFragment(neighbour);
    n->updateNeighbour(id, moveList[id]);
  }
}

void FragmentMgr::moveFragmentAccept(const ts::type::ID& id, NodeID nid) {
  auto it = std::find(movingFragmentAccept[id].begin(), movingFragmentAccept[id].end(), nid);
  movingFragmentAccept[id].erase(it);
  if(movingFragmentAccept[id].empty()) {
    messageMgr->sendFullFragment(moveList[id], findFragment(id));
  }
}

Fragment* FragmentMgr::findFragment(const ts::type::ID& id) {
  auto fragmentit = find_if(fragments.begin(), fragments.end(),
                            [&id](pair<Fragment*, bool> f) {
                              return *f.first == id;
                            });
  if(fragmentit == fragments.end())
    return 0;
  else
    return fragmentit->first;
}

}}