#pragma once

#include <vector>
#include <algorithm>

#include "Particle.h"

namespace ts {
namespace type {
  template <class ID, class ReduceData, class ExternalInfo>
  class AbstractCell {
  public:
    // General
    AbstractCell(): _iteration(0) {}
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
    virtual ReduceData reduce() = 0;
    virtual ReduceData reduce(ReduceData data) = 0;

    //Serialization
    virtual ExternalInfo* serialize() = 0;
    virtual void deserialize(ExternalInfo* info) = 0;

    // Iteration state
  private:
    ExternalInfo* state[2];
    int _iteration;

  public:
    int iteration() { 
      return _iteration;
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
    }

    void realRun() {
      run();
      nextIteration();
    }
  };
}}
