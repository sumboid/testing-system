#pragma once

#include <utility>
#include <vector>
#include <map>

#include "ID.h"
#include "ReduceData.h"

namespace ts {
namespace type {
typedef int NodeID;

/**
 * @class AbstractCell
 * @brief Cell abstract definition
 * This class contains low-level methods to manipulate
 * cell in system, and some virtual methods defined by user in
 * future.
 */

class AbstractCell {
private:
  ID _vid;                                   ///< ID of cell
  NodeID _vnodeID;                           ///< Logic cell location
  std::map<ID, NodeID> _vneighboursLocation; ///< Cell's neighbours location

  bool _vreduce;                             ///< Flag that indicate cell need for reduce step
  bool _vreduced;                            ///< Flag that indicate cell's reduce data was used
  bool _vupdate;                             ///< Flag that indicate external cell data need to update on remote nodes
  bool _vneighbours;                         ///< Flag that indicate cell need neighbours for next step
  bool _vend;                                ///< Flag that indicate cell is ready to end

  size_t _viteration;                        ///< Current iteration
  size_t _vprogress;                         ///< Current progress of iteration
public:
  // General
  AbstractCell(ID id);
  virtual ~AbstractCell();

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

  virtual void update(AbstractCell*) = 0;

  // Cell steps defined by user
  virtual ReduceData* reduce() = 0;
  virtual ReduceData* reduce(ReduceData* data) = 0;
  virtual void reduceStep(ReduceData* data) = 0;
  virtual void runStep(std::vector<AbstractCell*> neighbours) = 0;

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

  // State getters
  size_t iteration();
  size_t progress();

  // Low-level methods that changes state implicitly
  ReduceData* _reduce();
  ReduceData* _reduce(ReduceData* data);
  void _reduceStep(ReduceData* data);
  void _runStep(std::vector<AbstractCell*> neighbours);

  //Misc
  bool operator==(const AbstractCell& other);
  bool operator==(const ID& other);

};

}}
