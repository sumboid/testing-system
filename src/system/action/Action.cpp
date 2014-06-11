#include "Action.h"

using ts::NodeID;
namespace ts {
namespace system {

void Action::setSystem(System* _system) {
  system = _system;
}

void Action::setFragmentMgr(FragmentMgr* _fragmentMgr) {
  fragmentMgr = _fragmentMgr;
}

void Action::setExecMgr(ExecMgr* _execMgr) {
  execMgr = _execMgr;
}

void Action::setFragmentTools(ts::type::FragmentTools* _fragmentTools) {
  fragmentTools = _fragmentTools;
}

void Action::setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools) {
  reduceDataTools = _reduceDataTools;
}

}
}
