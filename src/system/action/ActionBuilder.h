#pragma once
#include <map>

#include "Action.h"
#include "../../types/CellTools.h"
#include "../../types/ReduceDataTools.h"
#include "../message/MessageMgr.h"

namespace ts {
namespace system {

class Action;
class Message;

class ActionBuilder {

private:
  System*     system;
  CellMgr*    cellMgr;
  ExecMgr*    execMgr;

  ts::type::CellTools* cellTools;
  ts::type::ReduceDataTools* reduceDataTools;

  std::map<int, Action*> actionMap;

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
