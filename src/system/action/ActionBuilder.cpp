#include <utility>
#include "ActionBuilder.h"
#include "actions/Update.h"
#include "actions/Reduce.h"
#include "actions/StartMove.h"
#include "actions/ConfirmMove.h"
#include "actions/GetFragment.h"

namespace ts {
namespace system {

using std::pair;

ActionBuilder::ActionBuilder() {
  actionMap.insert(pair<Tag, Action*>(Tag::UPDATE_FRAGMENT, new action::Update));
  actionMap.insert(pair<Tag, Action*>(Tag::REDUCE_DATA, new action::Reduce));
  actionMap.insert(pair<Tag, Action*>(Tag::START_MOVE_FRAGMENT, new action::StartMove));
  actionMap.insert(pair<Tag, Action*>(Tag::CONFIRM_MOVE_FRAGMENT, new action::ConfirmMove));
  actionMap.insert(pair<Tag, Action*>(Tag::MOVE_FRAGMENT, new action::GetFragment));
}

ActionBuilder::~ActionBuilder() {
  for(auto i: actionMap)
    delete i.second;
}

void ActionBuilder::setSystem(System* _system) {
  system = _system;
}

void ActionBuilder::setFragmentMgr(FragmentMgr* _fragmentMgr) {
  fragmentMgr = _fragmentMgr;
}

void ActionBuilder::setExecMgr(ExecMgr* _execMgr) {
  execMgr = _execMgr;
}

void ActionBuilder::setFragmentTools(ts::type::FragmentTools* _fragmentTools) {
  fragmentTools = _fragmentTools;
}

void ActionBuilder::setReduceDataTools(ts::type::ReduceDataTools* _reduceDataTools) {
  reduceDataTools = _reduceDataTools;
}

Action* ActionBuilder::build(const Message& message) {
  Action* action = actionMap[message.tag]->copy();

  action->setSystem(system);
  action->setFragmentMgr(fragmentMgr);
  action->setExecMgr(execMgr);
  action->setFragmentTools(fragmentTools);
  action->setReduceDataTools(reduceDataTools);

  action->set(message.buffer, message.size, message.node);

  return action;
}

}
}
