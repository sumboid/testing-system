#include "Fragment.h"
#include <algorithm>
#include <iostream>
using std::map;
using std::vector;
using std::tuple;
using std::pair;
using std::all_of;
using std::for_each;
using std::set;


namespace ts {
namespace type {
typedef int NodeID;

Fragment::Fragment(ID id) {
  _viteration = 0;
  _vprogress = 0;
  _vreduce = false;
  _vreduced = true;
  _vneighbours = false;
  _vid = id;
  _vupdate = false;
  _vend = false;
}

Fragment::~Fragment() {}

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


void Fragment::addNeighbour(ID id, NodeID node) {
  _vneighboursLocationMutex.lock();
  _vneighboursLocation.insert(std::pair<ID, NodeID>(id, node));
  _vneighboursLocationMutex.unlock();
  //_vneighboursLocation[id] = node;
}

void Fragment::updateNeighbour(ID id, NodeID node) {
  _vneighboursLocationMutex.lock();
  _vneighboursLocation[id] = node;
  _tryRemoveAllStates();
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

bool Fragment::needReduce() { return _vreduce;  }
bool Fragment::wasReduced() { return _vreduced; }
bool Fragment::needUpdate() { return _vupdate; }


//void Fragment::_serialize(void*& buf, size_t& size) {
//  _vupdate = false;
//  return serialize(buf, size);
//}

uint64_t Fragment::iteration() {
  return _viteration;
}

uint64_t Fragment::progress() {
  return _vprogress;
}

void Fragment::iteration(uint64_t i) {
  _viteration = i;
}

void Fragment::progress(uint64_t p) {
  _vprogress = p;
}

void Fragment::nextIteration() {
  ++_viteration;
  _vprogress = 0;
}

void Fragment::_runStep(std::vector<Fragment*> neighbours) {
  runStep(neighbours);
  ++_vprogress;
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
  _vlaststate = getBoundary();
  _vstates.insert(pair<Timestamp, Fragment*>(Timestamp(_viteration, _vprogress), _vlaststate));
}

void Fragment::saveState(Fragment* fragment) {
  _vstates.insert(pair<Timestamp, Fragment*>(Timestamp(fragment->iteration(), fragment->progress()), fragment));
}

Fragment* Fragment::getLastState() {
  _vupdate = false;
  return _vlaststate;
}

Fragment* Fragment::getState(const Timestamp& timestamp, const ID& neighbour) {
  _vstatesMutex.lock();
  Fragment* fragment = _vstates.at(timestamp);
  _vstateGetted[timestamp].insert(neighbour);
  _tryRemoveState(timestamp);
  _vstatesMutex.unlock();
  return fragment;
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

void Fragment::_tryRemoveState(Timestamp timestamp) {
  auto checkList = _vstateGetted[timestamp];
  if(any_of(_vneighboursLocation.begin(), _vneighboursLocation.end(), 
      [=](const pair<ID, NodeID>& neighbour) {
        if(neighbour.second == _vnodeID && checkList.find(neighbour.first) != checkList.end())
          return true;
        else
          return false;
      })
    ) {
    auto finded = _vstates.find(timestamp);
    delete finded->second;
    _vstates.erase(finded);
  }
}

void Fragment::_tryRemoveAllStates() {
  _vstatesMutex.lock();
  for_each(_vstates.begin(), _vstates.end(),
          [=](const pair<Timestamp, Fragment*>& state) {
          _tryRemoveState(state.first);
  });
  _vstatesMutex.unlock();
}

bool Fragment::operator==(const Fragment& other) { return _vid == other._vid; }
bool Fragment::operator==(const ID& other) { return _vid == other; }

#ifdef NDEBUG
  void Fragment::printStates() {
    if(_vstates.empty()) {
      std::cout << "NO STATES LOLD" << std::endl;
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
}
}}