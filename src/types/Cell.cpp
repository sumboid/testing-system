#include "Cell.h"
#include <algorithm>
#include <iostream>
using std::map;
using std::vector;
using std::tuple;
using std::pair;
using std::all_of;
using std::for_each;


namespace ts {
namespace type {
typedef int NodeID;

Cell::Cell(ID id) {
  _viteration = 0;
  _vprogress = 0;
  _vreduce = false;
  _vreduced = true;
  _vneighbours = false;
  _vid = id;
  _vupdate = false;
  _vend = false;
}

Cell::~Cell() {}

ID Cell::id() { return _vid; }
void Cell::setNodeID(NodeID nodeID) {
  _vnodeID = nodeID;
}

vector<NodeID> Cell::noticeList() {
  vector<NodeID> result;
  for(auto neighbour: _vneighboursLocation) {
    if(neighbour.second != _vnodeID)
      result.push_back(neighbour.second);
  }
  return result;
}

vector<ID> Cell::neighbours() {
  vector<ID> result;
  for(auto neighbour: _vneighboursLocation)
      result.push_back(neighbour.first);
  return result;
}

void Cell::updateNeighbour(ID id, NodeID node) {
  _vneighboursLocation.insert(std::pair<ID, NodeID>(id, node));
  //_vneighboursLocation[id] = node;
}

ReduceData* Cell::_reduce() {
  _vreduce = false;
  _vreduced = false;
  return reduce();
}

ReduceData* Cell::_reduce(ReduceData* data) {
  _vreduce = false;
  _vreduced = false;
  return reduce(data);
}

void Cell::_reduceStep(ReduceData* data) {
  _vreduced = true;
  reduceStep(data);
  ++_vprogress;
}

bool Cell::needReduce() { return _vreduce;  }
bool Cell::wasReduced() { return _vreduced; }
bool Cell::needUpdate() { return _vupdate; }


//void Cell::_serialize(void*& buf, size_t& size) {
//  _vupdate = false;
//  return serialize(buf, size);
//}

uint64_t Cell::iteration() {
  return _viteration;
}

uint64_t Cell::progress() {
  return _vprogress;
}

void Cell::iteration(uint64_t i) {
  _viteration = i;
}

void Cell::progress(uint64_t p) {
  _vprogress = p;
}

void Cell::nextIteration() {
  ++_viteration;
  _vprogress = 0;
}

void Cell::_runStep(std::vector<Cell*> neighbours) {
  runStep(neighbours);
  ++_vprogress;
}

void Cell::setEnd() {
  _vend = true;
}

void Cell::setReduce() {
  _vreduce = true;
}

void Cell::setUpdate() {
  _vupdate = true;
}

void Cell::setNeighbours(uint64_t iteration, uint64_t progress) {
  _vneighboursState = Timestamp(iteration, progress);
  _vneighbours = true;
}

bool Cell::isEnd() {
  return _vend;
}

bool Cell::needNeighbours() {
  return _vneighbours;
}

Timestamp Cell::neighboursState() {
  return _vneighboursState;
}

void Cell::saveState() {
  _vlaststate = getBoundary();
  _vstates.insert(pair<Timestamp, Cell*>(Timestamp(_viteration, _vprogress), _vlaststate));
}

void Cell::saveState(Cell* cell) {
  _vstates.insert(pair<Timestamp, Cell*>(Timestamp(cell->iteration(), cell->progress()), cell));
}

Cell* Cell::getLastState() {
  _vupdate = false;
  return _vlaststate;
}

Cell* Cell::getState(const Timestamp& timestamp, const ID& neighbour) {
  Cell* cell = _vstates.at(timestamp);
  _vstateGetted[timestamp].insert(neighbour);
  _tryRemoveState(timestamp);
  return cell;
}

bool Cell::hasState(const Timestamp& timestamp) {
  auto stateit = _vstates.find(timestamp);
  if(stateit == _vstates.end())
    return false;
  else
    return true;
}

void Cell::_tryRemoveState(Timestamp timestamp) {
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

void Cell::_tryRemoveAllStates() {
  for_each(_vstates.begin(), _vstates.end(),
          [=](const pair<Timestamp, Cell*>& state) {
          _tryRemoveState(state.first);
  });
}

bool Cell::operator==(const Cell& other) { return _vid == other._vid; }
bool Cell::operator==(const ID& other) { return _vid == other; }

#ifdef NDEBUG
  void Cell::printStates() {
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

}}
