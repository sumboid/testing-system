#include "Action.h"

namespace ts {
namespace system {

void Action::setSystem(System* _system) {
  system = _system;
}

void Action::setCellMgr(CellMgr* _cellMgr) {
  cellMgr = _cellMgr;
}

void Action::setExecMgr(ExecMgr* _execMgr) {
  execMgr = _execMgr;
}

void Action::setCellTools(ts::type::CellTools* _cellTools) {
  cellTools = _cellTools;
}

void Action::setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools) {
  reduceDataTools = _reduceDataTools;
}

}
}
