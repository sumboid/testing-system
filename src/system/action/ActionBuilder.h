#pragma once
#include <map>

#include "Action.h"
#include "../../types/CellTools.h"
#include "../../types/ReduceDataTools.h"

namespace ts {
namespace system {

class ActionBuilder {
private:
  System*     system;
  MessageMgr* msgMgr;
  CellMgr*    cellMgr;
  ExecMgr*    execMgr;

  ts::type::CellTools* cellTools;
  ts::type::ReduceDataTools* reduceDataTools;

  std::map<Tag, Action*> actionMap;

public:
  ActionBuilder();
  ~ActionBuilder();

  void setSystem(System* _system);
  void setCellMgr(CellMgr* _cellMgr);
  void setExecMgr(ExecMgr* _execMgr);
  void setCellTools(ts::type::CellTools* _cellTools);
  void setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools);

  Action* build(const Message& message);
};
}
}
