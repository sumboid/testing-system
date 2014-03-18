#pragma once

#include <utility>
#include <vector>
#include <map>
#include <set>
#include <tuple>
#include <cstdint>

#include "ID.h"
#include "ReduceData.h"
#include "CellTools.h"

namespace ts {
namespace type {
typedef int NodeID;

/**
 * @class Cell
 * @brief Cell abstract definition
 * This class contains low-level methods to manipulate
 * cell in system, and some virtual methods defined by user in
 * future.
 */

typedef std::tuple<uint64_t, uint64_t> Timestamp;
class Cell {
  friend class CellTools;
private:
  ID _vid;                                   ///< ID of cell
  NodeID _vnodeID;                           ///< Logic cell location
  std::map<ID, NodeID> _vneighboursLocation; ///< Cell's neighbours location

  bool _vreduce;                             ///< Flag that indicate cell need for reduce step
  bool _vreduced;                            ///< Flag that indicate cell's reduce data was used
  bool _vupdate;                             ///< Flag that indicate external cell data need to update on remote nodes
  bool _vneighbours;                         ///< Flag that indicate cell need neighbours for next step
  bool _vend;                                ///< Flag that indicate cell is ready to end

  uint64_t _viteration;                        ///< Current iteration
  uint64_t _vprogress;                         ///< Current progress of iteration

  std::map<Timestamp, Cell*> _vstates;                ///< stored states of cell
  Cell* _vlaststate;                                  ///< last state
  std::map<Timestamp, std::set<ID>> _vstateGetted;    ///< states getted by neighbours
public:
  // General
  Cell(ID id);
  virtual ~Cell();

  ID id();
  void setNodeID(NodeID);

  // Neighbours and their location
  std::vector<NodeID> noticeList();
  std::vector<ID> neighbours();
  void updateNeighbour(ID id, NodeID node);

  //Serialization external data
  virtual void serialize(void*& buf, size_t& size) = 0;
  virtual void deserialize(void* buf, size_t size) = 0;

  // ID(int, int, int) (iteration, progress) (size) (data) from serialize()
  void _serialize(void*& buf, size_t& size);

  virtual void update(Cell*) = 0;

  // Cell steps defined by user
  virtual ReduceData* reduce() = 0;
  virtual ReduceData* reduce(ReduceData* data) = 0;
  virtual void reduceStep(ReduceData* data) = 0;
  virtual void runStep(std::vector<Cell*> neighbours) = 0;

  // Flag setters
  void setEnd();
  void setUpdate();
  void setReduce();
  void setNeighbours();

  // State setters
  void nextIteration();

  // Flag getters
  bool isEnd();
  bool needReduce();
  bool wasReduced();
  bool needUpdate();
  bool needNeighbours();

  // Saving state
  virtual Cell* getBoundary() = 0;
  void saveState();
  void saveState(Cell* cell);
  Cell* getState(Timestamp timestamp, const ID& neighbour);
  Cell* getLastState();
  void _tryRemoveState(Timestamp timestamp);
  void _tryRemoveAllStates();

  // State getters
  uint64_t iteration();
  uint64_t progress();

  // Low-level methods that changes state implicitly
  ReduceData* _reduce();
  ReduceData* _reduce(ReduceData* data);
  void _reduceStep(ReduceData* data);
  void _runStep(std::vector<Cell*> neighbours);

  //Misc
  bool operator==(const Cell& other);
  bool operator==(const ID& other);

};

}}
