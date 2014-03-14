#include "AbstractCell.h"

using std::map;
using std::vector;

namespace ts {
namespace type {
typedef int NodeID;

AbstractCell::AbstractCell(ID id) {
  _viteration = 0;
  _vprogress = 0;
  _vreduce = false;
  _vreduced = true;
  _vid = id;
  _vneedUpdate = false;
  _vend = false;
}

AbstractCell::~AbstractCell() {}

ID AbstractCell::id() { return _vid; }
void AbstractCell::setNodeID(NodeID nodeID) {
  _vnodeID = nodeID;
}

vector<NodeID> AbstractCell::noticeList() {
  vector<NodeID> result;
  for(auto neighbour: _vneighboursLocation) {
    if(neighbour.second != _vnodeID)
      result.push_back(neighbour.second);
  }
  return result;
}

vector<ID> AbstractCell::neighbours() {
  vector<ID> result;
  for(auto neighbour: _vneighboursLocation)
      result.push_back(neighbour.first);
  return result;
}

void AbstractCell::updateNeighbour(ID id, NodeID node) {
  _vneighboursLocation[id] = node;
}

ReduceData* AbstractCell::_reduce() {
  _vreduce = false;
  _vreduced = false;
  return reduce();
}

ReduceData* AbstractCell::_reduce(ReduceData* data) {
  _vreduce = false;
  _vreduced = false;
  return reduce(data);
}

void AbstractCell::_reduceStep(ReduceData* data) {
  _vreduced = true;
  reduceStep(data);
  ++_vprogress;
}

bool AbstractCell::needReduce() { return _vreduce;  }
bool AbstractCell::wasReduced() { return _vreduced; }
bool AbstractCell::needUpdate() { return _vupdate; }


void AbstractCell::_serialize(void*& buf, size_t& size) {
  _vupdate = false;
  return serialize(buf, size);
}

size_t AbstractCell::iteration() {
  return _viteration;
}

size_t AbstractCell::progress() {
  return _vprogress;
}


void AbstractCell::nextIteration() {
  ++_viteration;
  _vprogress = 0;
}

void AbstractCell::_runStep(std::vector<AbstractCell*> neighbours) {
  runStep(neighbours);
  ++_vprogress;
}

void AbstractCell::setEnd() {
  _vend = true;
}

void AbstractCell::setReduce() {
  _vreduce = true;
}

void AbstractCell::setUpdate() {
  _vupdate = true;
}

void AbstractCell::setNeighbours() {
  _vneighbours = true;
}

bool AbstractCell::isEnd() {
  return _vend;
}

bool AbstractCell::operator==(const AbstractCell& other) { return _vid == other._vid; }
bool AbstractCell::operator==(const ID& other) { return _vid == other; }

}}
