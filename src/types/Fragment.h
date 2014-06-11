#pragma once

#include <utility>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <cstdint>
#include <mutex>

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
  std::mutex _vneighboursLocationMutex;

  bool _vreduce = false;                             ///< Flag that indicate fragment need for reduce step
  bool _vreduced = true;                            ///< Flag that indicate fragment's reduce data was used
  bool _vupdate = false;                             ///< Flag that indicate external fragment data need to update on remote nodes
  bool _vneighbours = false;                         ///< Flag that indicate fragment need neighbours for next step
  bool _vend = false;                                ///< Flag that indicate fragment is ready to end

  uint64_t _viteration = 0;                      ///< Current iteration
  uint64_t _vprogress = 0;                       ///< Current progress of iteration

  bool _visboundary = false;

  Timestamp _vneighboursState;
  std::map<Timestamp, Fragment*> _vstates;   ///< stored states of fragment
  std::mutex _vstatesMutex;

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

  // Neighbours and their location
  std::set<ts::NodeID> noticeList();
  std::vector<ID> neighbours();
  std::vector<ID> neighbours(ts::NodeID node);
  bool isNeighbour(const ID& id);
  void updateNeighbour(ID id, ts::NodeID node);
  std::vector<Fragment*> specialUpdateNeighbour(const ID& id, ts::NodeID node);
  void addNeighbour(ID id, ts::NodeID node);

  // Fragment steps defined by user
  virtual ReduceData* reduce() = 0;
  virtual ReduceData* reduce(ReduceData* data) = 0;
  virtual void reduceStep(ReduceData* data) = 0;
  virtual void runStep(std::vector<Fragment*> neighbours) = 0;

  // Flag setters
  void setEnd();
  void setUpdate();
  void setReduce();
  void setNeighbours(uint64_t iteration, uint64_t progress);

  // State setters
  void nextIteration();

  // Flag getters
  bool isEnd();
  bool needReduce();
  bool wasReduced();
  bool needUpdate();
  bool needNeighbours();

  Timestamp neighboursState();

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

  virtual uint64_t weight() = 0;
};

}}
