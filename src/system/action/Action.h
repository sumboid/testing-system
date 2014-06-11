#pragma once
#include "../System.h"
#include "../fragment/FragmentMgr.h"
#include "../exec/ExecMgr.h"
#include "../../types/FragmentTools.h"
#include "../../types/ReduceDataTools.h"
#include "../../util/Arc.h"
#include "../message/NodeID.h"

namespace ts {
namespace system {

class FragmentMgr;
class ExecMgr;
class System;
class Action {
protected:
  System*  system;
  FragmentMgr* fragmentMgr;
  ExecMgr* execMgr;

  ts::type::FragmentTools* fragmentTools;
  ts::type::ReduceDataTools* reduceDataTools;
public:
  virtual ~Action() {}
  virtual void run() = 0;
  virtual Action* copy() = 0;
  virtual void set(ts::Arc* arc, ts::NodeID id) = 0;

  void setSystem(System* _system);
  void setFragmentMgr(FragmentMgr* _fragmentMgr);
  void setExecMgr(ExecMgr* _execMgr);
  void setFragmentTools(ts::type::FragmentTools* _fragmentTools);
  void setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools);
};

}
}
