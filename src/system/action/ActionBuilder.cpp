#include <utility>
#include "ActionBuilder.h"
#include "actions/Update.h"
#include "actions/Reduce.h"

namespace ts {
namespace system {

using std::pair;

ActionBuilder::ActionBuilder() {
  actionMap.insert(pair<Tag, Action*>(UPDATE_CELL, new action::Update));
  actionMap.insert(pair<Tag, Action*>(REDUCE_DATA, new action::Reduce));
}

ActionBuilder::~ActionBuilder() {
  for(auto i: actionMap)
    delete i.second;
}

void ActionBuilder::setSystem(System* _system) {
  system = _system;
}

void ActionBuilder::setCellMgr(CellMgr* _cellMgr) {
  cellMgr = _cellMgr;
}

void ActionBuilder::setExecMgr(ExecMgr* _execMgr) {
  execMgr = _execMgr;
}

void ActionBuilder::setCellTools(ts::type::CellTools* _cellTools) {
  cellTools = _cellTools;
}

void ActionBuilder::setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools) {
  reduceDataTools = _reduceDataTools;
}

Action* ActionBuilder::build(const Message& message) {
  Action* action = actionMap[message.tag]->copy();

  action->setSystem(system);
  action->setCellMgr(cellMgr);
  action->setExecMgr(execMgr);
  action->setCellTools(cellTools);
  action->setReduceDataTools(reduceDataTools);

  action->set(message.buffer, message.size, message.node);

  return action;
}

}
}
