#pragma once

#include <utility>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <cstdint>
#include <mutex>
#include <iostream>

#include "ID.h"
#include "ReduceData.h"
#include "../util/RWLock.h"
#include "../system/message/NodeID.h"

namespace ts {

namespace system {
class FragmentMgr;
}

namespace type {

namespace util {
class FragmentDeserializer;
class FragmentSerializer;
}

class FragmentTools;

/**
 * @class Fragment
 * @brief Fragment abstract definition
 * This class contains low-level methods to manipulate
 * fragment in system, and some virtual methods defined by user in
 * future.
 */

typedef std::tuple<uint64_t, uint64_t> Timestamp;
class Fragment {
  friend class FragmentTools;
  friend class util::FragmentSerializer;
  friend class util::FragmentDeserializer;
  friend class ts::system::FragmentMgr;
private:
  ID _vid;                                   ///< ID of fragment
  ts::NodeID _vnodeID = 0;                           ///< Logic fragment location
  std::map<ID, ts::NodeID> _vneighboursLocation; ///< Fragment's neighbours location
  std::map<ID, ts::NodeID> _vvneighboursLocation; ///< Fragment's neighbours location
  std::mutex _vneighboursLocationMutex;
  std::mutex _vvneighboursLocationMutex;

  bool _vreduce = false;                             ///< Flag that indicate fragment need for reduce step
  bool _vreduced = true;                            ///< Flag that indicate fragment's reduce data was used
  bool _vupdate = false;                             ///< Flag that indicate external fragment data need to update on remote nodes
  bool _vneighbours = false;                         ///< Flag that indicate fragment need neighbours for next step
  bool _vend = false;                                ///< Flag that indicate fragment is ready to end
  bool _vhalt = false;
  bool _vmovable = true;

  bool _vmaster = false;
  bool _vvneighbours = false; // Need virtual neighbours
  bool _vvupdate = false;
  bool _vvirtual = false;

  uint64_t _viteration = 0;                      ///< Current iteration
  uint64_t _vprogress = 0;                       ///< Current progress of iteration
  uint64_t _vvcounter = 0;

  uint64_t loadChange;
  bool _visboundary = false;

  Timestamp _vneighboursState;
  std::map<Timestamp, Fragment*> _vstates;   ///< stored states of fragment
  std::mutex _vstatesMutex;

  Timestamp _vvneighboursState;
  Fragment* _vlaststate = 0;                           ///< last state
  bool _vlaststateWasSaved = false;
  std::map<Timestamp, std::set<ID>> _vstateGetted; ///< states getted by neighbours
  ts::RWLock _vstateGettedLock;
public:
  // General
  Fragment(ID id);
  virtual ~Fragment();

  ID id();
  void setNodeID(ts::NodeID);
  ts::NodeID nodeID() { return _vnodeID; }

  // Neighbours and their location
  std::set<ts::NodeID> noticeList();
  std::vector<ID> neighbours();
  std::vector<ID> neighbours(ts::NodeID node);
  bool isNeighbour(const ID& id);
  void updateNeighbour(ID id, ts::NodeID node);
  std::vector<Fragment*> specialUpdateNeighbour(const ID& id, ts::NodeID node);
  void addNeighbour(ID id, ts::NodeID node);

  std::set<ts::NodeID> vnoticeList();
  std::vector<ID> vneighbours();
  std::vector<ID> vneighbours(ts::NodeID node);
  bool isVNeighbour(const ID& id);
  void updateVNeighbour(ID id, ts::NodeID node);
  std::vector<Fragment*> specialUpdateVNeighbour(const ID& id, ts::NodeID node);
  void addVNeighbour(ID id, ts::NodeID node);

  // Fragment steps defined by user
  virtual ReduceData* reduce() = 0;
  virtual ReduceData* reduce(ReduceData* data) = 0;
  virtual void reduceStep(ReduceData* data) = 0;
  virtual void runStep(std::vector<Fragment*> neighbours) = 0;

  // Flag setters
  void setEnd();
  void setHalt();
  void setNotMovable();
  void setUpdate();
  void setVUpdate();
  void setReduce();
  void setNeighbours(uint64_t iteration, uint64_t progress);
  void setVNeighbours(uint64_t, uint64_t);

  // State setters
  void nextIteration();

  // Flag getters
  bool isEnd();
  bool isHalt();
  bool isMovable();
  bool isVirtual();
  bool isMaster() { return _vmaster; }
  bool needReduce();
  bool wasReduced();
  bool needUpdate();
  bool needVUpdate();
  bool needNeighbours();
  bool needVNeighbours();

  Timestamp neighboursState();
  Timestamp vneighboursState();

  // Saving state
  virtual Fragment* getBoundary() = 0;
  void createExternal(Fragment* f);
  void saveState();
  void saveState(Fragment* fragment);
  void moveStates(Fragment* fragment);
  Fragment* getState(const Timestamp& timestamp, const ID& neighbour);
  Fragment* getLastState();
  bool hasState(const Timestamp& timestamp);
  void _tryRemoveState(Timestamp timestamp);
  bool _stateCanBeRemoved(Timestamp timestamp);
  void _tryRemoveAllStates();

  // State getters
  uint64_t iteration();
  uint64_t progress();

  // Checking boundary
  bool isBoundary() {
    return _visboundary;
  }

  void setBoundary() {
    _visboundary = true;
  }

  // Low-level methods that changes state implicitly
  ReduceData* _reduce();
  ReduceData* _reduce(ReduceData* data);
  void _reduceStep(ReduceData* data);
  void _runStep(std::vector<Fragment*> neighbours);

  //Misc
  bool operator==(const Fragment& other);
  bool operator==(const ID& other);

#ifdef NDEBUG
  void printStates();
#endif

  bool equal(Fragment* another);
  virtual bool _equal(Fragment* another);
  virtual Fragment* copy() = 0;
  void print();

  Fragment* _split() {
      Fragment* s = split();
      this->_vmaster = true;
      ++_vvcounter;
      ID vid = ID(id().c[0], id().c[1], id().c[2], _vvcounter);
      s->addNeighbour(id(), _vnodeID);
      s->_vvirtual = true;
      s->_vvneighboursLocation.emplace(id(), _vnodeID);
      s->_vid = vid;
      s->_viteration = _viteration;
      s->_vprogress = _vprogress;

      addVNeighbour(vid, _vnodeID);
      return s;
  }

  void _merge(Fragment* f) {
      --_vvcounter;
      if(_vvcounter == 0) {
          _vmaster = false;
      }

      ID finded;
      for(auto &vn : _vvneighboursLocation) {
        if(vn.first == f->id()) {
            finded = vn.first;
            break;
        }
      }

      _vvneighboursLocation.erase(finded);
      merge(f);
  }

  virtual uint64_t weight() = 0;
  virtual Fragment* split() = 0;
  virtual void merge(Fragment*) = 0;
  virtual bool canSplit() = 0;
  virtual bool canMove() = 0;

  int changeLoad() {
      return loadChange;
  }
};

}}
