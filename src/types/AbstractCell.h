#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <map>

#include "ID.h"
#include "ReduceData.h"

namespace ts {
namespace type {
typedef int NodeID;

class AbstractCell {
protected:
    ID _id;
public:
  // General
  AbstractCell(ID id);
  virtual ~AbstractCell();
  virtual void run(std::vector<AbstractCell*>) = 0;
  ID id();

  // Neighbours and their location
private:
  NodeID nodeID;
  std::map<ID, NodeID> neighboursLocation;

public:
  std::vector<NodeID> noticeList();
  std::vector<ID> neighbours();
  void updateNeighbour(ID id, NodeID node);

  // Reduce
protected:
  bool _vreduce;
  bool _vreduced;

public:
  virtual ReduceData* reduce() = 0;
  virtual ReduceData* reduce(ReduceData* data) = 0;

  virtual void reduceStep(ReduceData* data) = 0;

  ReduceData* _reduce();
  ReduceData* _reduce(ReduceData* data);

  void _reduceStep(ReduceData* data);
  bool needReduce();
  bool wasReduced();

  //Serialization external data
private:
  bool _vneedUpdate;

public:
  bool needUpdate();
  

  virtual void serialize(void*& buf, size_t& size) = 0;
  virtual void deserialize(void* buf, size_t size) = 0;

  void _serialize(void*& buf, size_t& size);

  virtual void update(AbstractCell*) = 0;
  // Iteration state
private:
  size_t _iteration;
  size_t _progress;

public:
  size_t iteration();
  size_t progress();

  void nextIteration();
  void _runStep(std::vector<AbstractCell*> neighbours);
  bool operator==(const AbstractCell& other);
  bool operator==(const ID& other);

  // End
private:
  bool _vend;

public:
  void end();
  bool isEnd();
};

}}
