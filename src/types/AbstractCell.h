#pragma once

#include <utility>
#include <vector>
#include <algorithm>

namespace ts {
namespace type {
  typedef int NodeID;
  class ID {
  public:
    virtual ~ID() {}
  };

  class ReduceData {
  public:
    virtual ~ReduceData() {}
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
  public:
    // General
    AbstractCell(): _iteration(0),
                    _progress(0),
                    _vreduce(false),
                    _vreduced(true) {}
    virtual ~AbstractCell() {}
    // virtual void addParticle(Particle* particle) = 0;
    // virtual void removeParticle(Particle* particle) = 0;

    virtual void run() = 0;
    virtual ID id() = 0;

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
      neighbours[id] = node;
    }

    // Reduce
  private:
    bool _vreduce;
    bool _vreduced;

  public:
    virtual ReduceData* reduce() = 0;
    virtual ReduceData* reduce(ReduceData* data) = 0;
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

    //Serialization
    virtual Data* serialize() = 0;
    virtual void deserialize(Data* info) = 0;

    // Iteration state
  private:
    Data* state[2];
    unsigned int _iteration;
    unsigned int _progress;

  public:
    unsigned int iteration() {
      return _iteration;
    }

    unsigned int progress() {
      return _progress;
    }

    Data* currentState() {
      return state[1];
    }

    Data* prevState() {
      return state[0];
    }

    void nextIteration() {
      if(_iteration > 1) delete state[0];
      state[0] = state[1];
      state[1] = serialize();
      ++_iteration;
      _progress = 0;
    }

    void _runStep() {
      run();
      ++_progress;
    }
  };

  class AbstractCellTools {
  public:
    virtual ~AbstractCellTools() {}
    virtual void serialize(AbstractCell* cell, void* buf, size_t size) = 0;
    virtual AbstractCell* deserialize(void*& buf, size_t& size) = 0;
  };
}}
