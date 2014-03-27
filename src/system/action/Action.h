#pragma once
#include "../System.h"
#include "../cell/CellMgr.h"
#include "../exec/ExecMgr.h"
#include "../../types/CellTools.h"
#include "../../types/ReduceDataTools.h"

namespace ts {
namespace system {

class Action {
protected:
  System*  system;
  CellMgr* cellMgr;
  ExecMgr* execMgr;

  ts::type::CellTools* cellTools;
  ts::type::ReduceDataTools* reduceDataTools;
public:
  virtual ~Action() {}
  virtual void run() = 0;
  virtual Action* copy() = 0;
  virtual void set(char* buffer, size_t size, ts::type::NodeID id) = 0;

  void setSystem(System* _system);
  void setCellMgr(CellMgr* _cellMgr);
  void setExecMgr(ExecMgr* _execMgr);
  void setCellTools(ts::type::CellTools* _cellTools);
  void setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools);
};

}
}
