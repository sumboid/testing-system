#pragma once
#include <map>

#include "Action.h"
#include "../../types/FragmentTools.h"
#include "../../types/ReduceDataTools.h"
#include "../message/MessageMgr.h"

namespace ts {
namespace system {

class Action;
class Message;

class ActionBuilder {

private:
  System*     system;
  FragmentMgr*    fragmentMgr;
  ExecMgr*    execMgr;

  ts::type::FragmentTools* fragmentTools;
  ts::type::ReduceDataTools* reduceDataTools;

  std::map<int, Action*> actionMap;

public:
  ActionBuilder();
  ~ActionBuilder();

  void setSystem(System* _system);
  void setFragmentMgr(FragmentMgr* _fragmentMgr);
  void setExecMgr(ExecMgr* _execMgr);
  void setFragmentTools(ts::type::FragmentTools* _fragmentTools);
  void setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools);

  Action* build(const Message& message);
};
}
}
