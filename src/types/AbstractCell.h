#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include <map>

namespace ts {
namespace type {
  typedef int NodeID;
  class ID {
  private:
    enum {X = 0, Y = 1, Z = 2};
    unsigned int c[3];
  public:
    ID(int x = 0, int y = 0, int z = 0) {
      c[X] = x;
      c[Y] = y;
      c[Z] = z;
    }

    virtual ~ID() {}
    bool operator<(const ID& other) const {
      return true; //XXX: Need to check map behaviour
    }
    bool operator>(const ID& other) const {
      return false; //XXX: Need to check map behaviour
    }
    bool operator==(const ID& other) {
      return c[X] == other.c[X] && c[Y] == other.c[Y] && c[Z] == other.c[Z];
    }
  };

  class ReduceData {
  public:
    virtual ~ReduceData() {}
  };

  class ReduceDataTools {
  public:
    virtual ~ReduceDataTools() {}
    virtual void serialize(ReduceData* data, char*& buf, size_t& size) = 0;
    virtual ReduceData* deserialize(void* buf, size_t size) = 0;
    virtual ReduceData* reduce(ReduceData*, ReduceData*) = 0;
  };

  class Data {
  public:
    virtual std::pair<void*, size_t> serialize();
  };

  class Deserializer {
  public:
    virtual Data* deserialize(void* buf, size_t size);
  };

  class AbstractCell {
  private:
      ID _id;
  public:
    // General
    AbstractCell(ID id) {
      _iteration = 0;
      _progress = 0;
      _vreduce = false;
      _vreduced = true;
      _id = id;
      _vneedUpdate = false;
    }
    virtual ~AbstractCell() {}
    // virtual void addParticle(Particle* particle) = 0;
    // virtual void removeParticle(Particle* particle) = 0;

    virtual void run(std::vector<AbstractCell*>) = 0;
    ID id() { return _id; }

    // Neighbours and their location
  private:
    NodeID nodeID;
    std::map<ID, NodeID> neighboursLocation;

  public:
    std::vector<NodeID> noticeList() {
      std::vector<NodeID> result;
      for(auto neighbour: neighboursLocation) {
        if(neighbour.second != nodeID)
          result.push_back(neighbour.second);
      }
      return result;
    }

    std::vector<ID> neighbours() {
      std::vector<ID> result;
      for(auto neighbour: neighboursLocation)
          result.push_back(neighbour.first);
      return result;
    }

    void updateNeighbour(ID id, NodeID node) {
      // TODO: check id
      neighboursLocation[id] = node;
    }

    // Reduce
  private:
    bool _vreduce;
    bool _vreduced;

  public:
    virtual ReduceData* reduce() = 0;
    virtual ReduceData* reduce(ReduceData* data) = 0;
    virtual ReduceData* reduce(ReduceData* data1, ReduceData* data2) = 0;

    virtual void reduceStep(ReduceData* data) = 0;

    ReduceData* _reduce() {
      _vreduce = false;
      _vreduced = false;
      return reduce();
    }

    ReduceData* _reduce(ReduceData* data) {
      _vreduce = false;
      _vreduced = false;
      return reduce(data);
    }

    void _reduceStep(ReduceData* data) {
      _vreduced = true;
      reduceStep(data);
      ++_progress;
    }

    bool needReduce() { return _vreduce;  }
    bool wasReduced() { return _vreduced; }

    //Serialization external data
  private:
    bool _vneedUpdate;

  public:
    bool needUpdate() { return _vneedUpdate; }

    virtual void serialize(void*& buf, size_t& size) = 0;
    virtual void deserialize(void* buf, size_t size) = 0;

    void _serialize(void*& buf, size_t& size) {
      _vneedUpdate = false;
      return serialize(buf, size);
    }

    virtual void update(AbstractCell*) = 0;
    // Iteration state
  private:
    size_t _iteration;
    size_t _progress;

  public:
    size_t iteration() {
      return _iteration;
    }

    size_t progress() {
      return _progress;
    }


    void nextIteration() {
      ++_iteration;
      _progress = 0;
    }

    void _runStep(std::vector<AbstractCell*> neighbours) {
      run(neighbours);
      ++_progress;
    }
    bool operator==(const AbstractCell& other) { return _id == other._id; }
    bool operator==(const ID& other) { return _id == other; }
  };

  class AbstractCellTools {
  public:
    virtual ~AbstractCellTools() {}
    virtual void serialize(AbstractCell* cell, void* buf, size_t size) = 0;
    virtual AbstractCell* deserialize(char*& buf, size_t& size) = 0;
  };
}}
