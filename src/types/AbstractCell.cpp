#include "AbstractCell.h"

using std::map;
using std::vector;

namespace ts {
namespace type {
typedef int NodeID;

AbstractCell::AbstractCell(ID id) {
  _iteration = 0;
  _progress = 0;
  _vreduce = false;
  _vreduced = true;
  _id = id;
  _vneedUpdate = false;
  _vend = false;
}
ID AbstractCell::id() { return _id; }

vector<NodeID> AbstractCell::noticeList() {
  vector<NodeID> result;
  for(auto neighbour: neighboursLocation) {
    if(neighbour.second != nodeID)
      result.push_back(neighbour.second);
  }
  return result;
}

vector<ID> AbstractCell::neighbours() {
  vector<ID> result;
  for(auto neighbour: neighboursLocation)
      result.push_back(neighbour.first);
  return result;
}

void AbstractCell::updateNeighbour(ID id, NodeID node) {
  // TODO: check id
  neighboursLocation[id] = node;
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
  ++_progress;
}

bool AbstractCell::needReduce() { return _vreduce;  }
bool AbstractCell::wasReduced() { return _vreduced; }
bool AbstractCell::needUpdate() { return _vneedUpdate; }


void AbstractCell::_serialize(void*& buf, size_t& size) {
  _vneedUpdate = false;
  return serialize(buf, size);
}

size_t AbstractCell::iteration() {
  return _iteration;
}

size_t AbstractCell::progress() {
  return _progress;
}


void AbstractCell::nextIteration() {
  ++_iteration;
  _progress = 0;
}

void AbstractCell::_runStep(std::vector<AbstractCell*> neighbours) {
  run(neighbours);
  ++_progress;
}
bool AbstractCell::operator==(const AbstractCell& other) { return _id == other._id; }
bool AbstractCell::operator==(const ID& other) { return _id == other; }

void AbstractCell::end() {
  _vend = true;
}

bool AbstractCell::isEnd() {
  return _vend;
}
}}
