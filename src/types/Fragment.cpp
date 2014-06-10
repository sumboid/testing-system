#include "Fragment.h"
#include "../util/Uberlogger.h"
#include <algorithm>
#include <iostream>
#include <cassert>
using std::map;
using std::vector;
using std::tuple;
using std::pair;
using std::all_of;
using std::for_each;
using std::set;
using ts::NodeID;


namespace ts {
namespace type {
typedef int NodeID;

Fragment::Fragment(ID id):
  _vid(id)
{}

Fragment::~Fragment() {
}

ID Fragment::id() { return _vid; }
void Fragment::setNodeID(NodeID nodeID) {
  _vnodeID = nodeID;
}

set<NodeID> Fragment::noticeList() {
  set<NodeID> result;
  _vneighboursLocationMutex.lock();
  for(auto neighbour: _vneighboursLocation) {
    if(neighbour.second != _vnodeID)
      result.insert(neighbour.second);
  }
  _vneighboursLocationMutex.unlock();
  return result;
}

vector<ID> Fragment::neighbours() {
  vector<ID> result;
  _vneighboursLocationMutex.lock();
  for(auto neighbour: _vneighboursLocation)
      result.push_back(neighbour.first);
  _vneighboursLocationMutex.unlock();
  return result;
}

vector<ID> Fragment::neighbours(NodeID node) {
  vector<ID> result;
  _vneighboursLocationMutex.lock();
  for(auto neighbour: _vneighboursLocation)
    if(neighbour.second == node)
      result.push_back(neighbour.first);
  _vneighboursLocationMutex.unlock();
  return result;
}


bool Fragment::isNeighbour(const ID& id) {
  for(auto n : _vneighboursLocation) {
    if(n.first == id)
      return true;
  }
  return false;
}
void Fragment::addNeighbour(ID id, NodeID node) {
  _vneighboursLocationMutex.lock();
  //UBERLOG() << _vid.tostr() << " Adding neighbour: " << id.tostr() << UBEREND();
  _vneighboursLocation.insert(std::pair<ID, NodeID>(id, node));
  _vneighboursLocationMutex.unlock();
  //_vneighboursLocation[id] = node;
}

void Fragment::updateNeighbour(ID id, NodeID node) {
  _vneighboursLocationMutex.lock();
  _vneighboursLocation[id] = node;
  _vneighboursLocationMutex.unlock();
}

ReduceData* Fragment::_reduce() {
  _vreduce = false;
  _vreduced = false;
  return reduce();
}

ReduceData* Fragment::_reduce(ReduceData* data) {
  _vreduce = false;
  _vreduced = false;
  return reduce(data);
}

void Fragment::_reduceStep(ReduceData* data) {
  _vreduced = true;
  reduceStep(data);
  ++_vprogress;
}

bool Fragment::needReduce() { return _vreduce; }
bool Fragment::wasReduced() { return _vreduced; }
bool Fragment::needUpdate() { return _vupdate; }

uint64_t Fragment::iteration() {
  return _viteration;
}

uint64_t Fragment::progress() {
  return _vprogress;
}

void Fragment::nextIteration() {
  ++_viteration;
  _vprogress = 0;
}

void Fragment::_runStep(std::vector<Fragment*> neighbours) {
  runStep(neighbours);
  ++_vprogress;
  if(!neighbours.empty()) {
    _vneighbours = false;
  }
}

void Fragment::setEnd() {
  _vend = true;
}

void Fragment::setReduce() {
  _vreduce = true;
}

void Fragment::setUpdate() {
  _vupdate = true;
}

void Fragment::setNeighbours(uint64_t iteration, uint64_t progress) {
  //ULOG(fragment) << _vid.tostr() << " [" << _viteration << ", " << _vprogress << "] " << "Set neighbours timestamp: " << iteration << ":" << progress << UEND;
  _vneighboursState = Timestamp(iteration, progress);
  _vneighbours = true;
}

bool Fragment::isEnd() {
  return _vend;
}

bool Fragment::needNeighbours() {
  return _vneighbours;
}

Timestamp Fragment::neighboursState() {
  return _vneighboursState;
}

void Fragment::saveState() {
  ULOG(state) << id().tostr() << " Saved state: (" << iteration() << ", " << progress() << ")" <<
    UEND;
  _vlaststateWasSaved = true;
  _vlaststate = getBoundary();
  _vlaststate->_viteration = _viteration;
  _vlaststate->_vprogress = _vprogress;
  _vlaststate->_visboundary = true;
  _vstates.insert(pair<Timestamp, Fragment*>(Timestamp(_viteration, _vprogress), _vlaststate));
}

void Fragment::saveState(Fragment* fragment) {
  ULOG(state) << id().tostr() << " Saved state: (" << fragment->iteration() << ", " << fragment->progress() << ")" <<
    " from " << fragment->id().tostr() << UBEREND();
  _vstatesMutex.lock();
  _vstates.insert(pair<Timestamp, Fragment*>(Timestamp(fragment->iteration(), fragment->progress()), fragment));
  _vstatesMutex.unlock();
  _vstateGettedLock.wlock();
  _vstateGetted.emplace(Timestamp(fragment->iteration(), fragment->progress()), std::set<ID>());
  _vstateGettedLock.unlock();
}

void Fragment::moveStates(Fragment* fragment) {
  _vstatesMutex.lock();
  for(auto state: _vstates) {
    fragment->saveState(state.second);
  }
  _vstatesMutex.unlock();
}

Fragment* Fragment::getLastState() {
  if(!_vlaststateWasSaved) {
    ULOG(error) << "Get last state from " << id().tostr() << ", but laststate was not saved, COOL" << UEND;
    exit(3);
  }

  Fragment* f = _vlaststate->getBoundary();
  f->_vid = id();
  f->_viteration = _vlaststate->_viteration;
  f->_vprogress = _vlaststate->_vprogress;
  ULOG(state) << "Get last state from " << id().tostr() << " with state: " << f->_viteration << UEND;
  _vneighboursLocationMutex.lock();
  for(auto &n : _vneighboursLocation) {
    f->addNeighbour(n.first, n.second);
  }
  _vneighboursLocationMutex.unlock();
  _vlaststateWasSaved = false;
  _vupdate = false;
  return f;
}

Fragment* Fragment::getState(const Timestamp& timestamp, const ID& neighbour) {
  _vstatesMutex.lock();
  Fragment* fragment = _vstates.at(timestamp)->getBoundary();

  _vstateGettedLock.wlock([&]() {
      _vstateGetted[timestamp].insert(neighbour);
  });

  //_tryRemoveState(timestamp);
  _vstatesMutex.unlock();
  return fragment;
}

vector<Fragment*> Fragment::specialUpdateNeighbour(const ID& neighbour,
                                                   NodeID node) {
  vector<Fragment*> result;
  _vstateGettedLock.rlock();
  _vstatesMutex.lock();
    for(auto i : _vstateGetted) {
      auto yoba = ULOG(error);
      yoba << _vid.tostr() << " at " << std::get<0>(i.first) << ":" << std::get<1>(i.first) << " was getted by: ";
      for(auto j : i.second) yoba << j.tostr() << " ";
      yoba << UEND;
      if(i.second.find(neighbour) == i.second.end()) {
        try {
          Fragment* ob = _vstates.at(i.first);
          Fragment* b = ob->getBoundary();
          b->_vid = _vid;
          b->_viteration = ob->_viteration;
          b->_vprogress = ob->_vprogress;
          for(auto& n : _vneighboursLocation) {
            if(!(n.first == neighbour)) {
              b->addNeighbour(n.first, n.second);
            } else b->addNeighbour(neighbour, node);
          }

          ULOG(error) << "SUN " << neighbour.tostr() << ": " << b->_vid.tostr() << " with iteration " << b->_viteration << UEND;
          result.push_back(b);
        } catch(...) {
          ULOG(error) << "Oh-oh" << UEND;
        }
      }
    }
  _vstatesMutex.unlock();
  _vstateGettedLock.unlock();
  _vneighboursLocationMutex.lock();
  _vneighboursLocation[neighbour] = node;
  _vneighboursLocationMutex.unlock();
  //_tryRemoveAllStates();
  return result;
}

bool Fragment::hasState(const Timestamp& timestamp) {
  _vstatesMutex.lock();
  auto stateit = _vstates.find(timestamp);
  _vstatesMutex.unlock();
  if(stateit == _vstates.end())
    return false;
  else
    return true;
}

bool Fragment::_stateCanBeRemoved(Timestamp timestamp) {
  _vstateGettedLock.rlock();
  auto checkList = _vstateGetted[timestamp];
  _vstateGettedLock.unlock();

  if(all_of(_vneighboursLocation.begin(), _vneighboursLocation.end(),
      [=](const pair<ID, NodeID>& neighbour) {
        if(neighbour.second != _vnodeID)
          return true;
        else if(checkList.find(neighbour.first) != checkList.end())
          return true;
        else
          return false;
      })
    ) {
    auto pek = ULOG(fragment) << _vid.tostr() << ": Remove state: (" << std::get<0>(timestamp) << ", " << std::get<1>(timestamp) << ") ";
    pek << "because state was getted by: ";
    for(auto &a : checkList) {
      pek << a.tostr() << " ";
    }

    pek << UEND;

    return true;
  }
  return false;
}

void Fragment::_tryRemoveState(Timestamp /*timestamp*/) {
//  if(_stateCanBeRemoved(timestamp)) {
//    delete _vstates.at(timestamp);
//    _vstates.erase(timestamp);

//    _vstateGettedLock.wlock();
//    _vstateGetted.erase(timestamp);
//    _vstateGettedLock.unlock();
//  }
}

void Fragment::_tryRemoveAllStates() {
//  vector<Timestamp> rmlist;
//  _vstatesMutex.lock();
//  for(auto& state: _vstates)
//    if(_stateCanBeRemoved(state.first))
//      rmlist.push_back(state.first);

//  _vstateGettedLock.wlock();
//  for(auto& timestamp : rmlist) {
//    delete _vstates.at(timestamp);
//    _vstates.erase(timestamp);
//    _vstateGetted.erase(timestamp);
//  }
//  _vstatesMutex.unlock();
//  _vstateGettedLock.unlock();
}

bool Fragment::operator==(const Fragment& other) { return _vid == other._vid; }
bool Fragment::operator==(const ID& other) { return _vid == other; }

#ifdef NDEBUG
  void Fragment::printStates() {
    if(_vstates.empty()) {
      return;
    }
    for(auto state: _vstates) {
      uint64_t i, p;
      std::tie(i, p) = state.first;
      std::cout << i << " : " << p << std::endl;
    }
  }
#endif

void Fragment::print() {
  std::cout << "Fragment(" << _vid.c[0] << ", " << _vid.c[1] << ", " << _vid.c[2] << "):"<< std::endl;
  std::cout << "  - State: (" << _viteration << ", " << _vprogress << ")" << std::endl;
}

void Fragment::createExternal(Fragment* f) {
  _vstatesMutex.lock();
  for(auto i: _vstates) {
    f->_vstates.insert(i);
  }
  _vstatesMutex.unlock();
  _vneighboursLocationMutex.lock();
  for(auto i: _vneighboursLocation) {
    f->_vneighboursLocation.emplace(i);
  }
}

bool Fragment::equal(Fragment* another) {
  if(!(_vid == another->_vid)) {
    UBERLOG() << "ID: " << _vid.tostr() << " != " << another->_vid.tostr() << UBEREND();
    return false;
  }
  if(_viteration != another->_viteration) return false;
  if(_vprogress != another->_vprogress) return false;
  if(_vneighboursLocation.size() != another->_vneighboursLocation.size()) return false;
  for(auto i : _vneighboursLocation) {
    auto j = another->_vneighboursLocation.find(i.first);
    if(j == another->_vneighboursLocation.end()) return false;
    if(j->second != i.second) return false;
  }
  if(_vreduce != another->_vreduce) return false;
  if(_vreduced != another->_vreduced) return false;
  if(_vupdate != another->_vupdate) return false;
  if(_vneighbours != another->_vneighbours) return false;
  if(_vend != another->_vend) return false;

  return _equal(another);
}

bool Fragment::_equal(Fragment*) {
  return true;
}

}}
