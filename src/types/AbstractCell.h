#pragma once

#include <vector>
#include <algorithm>

#include "Particle.h"

namespace ts {
namespace type {
  class AbstractCellTools {
  public:
    virtual ~AbstractCellTools() {}
    virtual void serialize(AbstractCell* cell, void* buf, size_t size) = 0;
    virtual AbstractCell* deserialize(void*& buf, size_t& size) = 0;
  };

  class ReduceData {
  public:
    virtual ~ReduceData() {}
  };

  template <class ID, class ExternalInfo>
  class AbstractCell {
  public:
    // General
    AbstractCell(): _iteration(0),
                    _progress(0),
                    _reduce(false),
                    _reduced(true) {}
    virtual ~AbstractCell() {}
    virtual void addParticle(Particle* particle) = 0;
    virtual void removeParticle(Particle* particle) = 0;

    virtual void run() = 0;
    virtual ID id() = 0;

    // Neighbours and their location
  private:
    NodeID nodeID;
    std::map<ID, NodeID> neighboursLocation;

  public:
    vector<NodeID> noticeList() {
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
    bool _reduce;
    bool _reduced;

  public:
    virtual ReduceData* reduce() = 0;
    virtual ReduceData* reduce(ReduceData* data) = 0;
    virtual void reduceStep(ReduceData* data) = 0;

    ReduceData* _reduce() {
      _reduce = false;
      _reduced = false;
      return reduce();
    }

    ReduceData* _reduce(ReduceData* data) {
      _reduce = false;
      _reduced = false;
      return reduce(data);
    }

    void _reduceStep(ReduceData* data) {
      _reduced = true;
      reduceStep(data);
      ++_progress;
    }

    bool needReduce() { return _reduce;  }
    bool wasReduced() { return _reduced; }
    //Serialization
    virtual ExternalInfo* serialize() = 0;
    virtual void deserialize(ExternalInfo* info) = 0;

    // Iteration state
  private:
    ExternalInfo* state[2];
    unsigned int _iteration;
    unsigned int _progress;

  public:
    unsigned int iteration() {
      return _iteration;
    }

    unsigned int progress() {
      return _progress;
    }

    ExternalInfo* currentState() {
      return state[1];
    }
    ExternalInfo* prevState() {
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
}}
