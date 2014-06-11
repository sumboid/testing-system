#include <utility>
#include "ActionBuilder.h"
#include "actions/Update.h"
#include "actions/Reduce.h"
#include "actions/StartMove.h"
#include "actions/NoticeMove.h"
#include "actions/ConfirmMove.h"
#include "actions/GlobalConfirmMove.h"
#include "actions/Move.h"
#include "actions/Load.h"

namespace ts {
namespace system {

using std::pair;

ActionBuilder::ActionBuilder():
  system(0),
  fragmentMgr(0),
  execMgr(0),
  fragmentTools(0),
  reduceDataTools(0) {
  actionMap.insert(pair<Tag, Action*>(Tag::UPDATE_FRAGMENT, new action::Update));
  actionMap.insert(pair<Tag, Action*>(Tag::REDUCE_DATA, new action::Reduce));
  actionMap.insert(pair<Tag, Action*>(Tag::START_MOVE_FRAGMENT, new action::StartMove));
  actionMap.insert(pair<Tag, Action*>(Tag::NOTICE_MOVE_FRAGMENT, new action::NoticeMove));
  actionMap.insert(pair<Tag, Action*>(Tag::CONFIRM_MOVE_FRAGMENT, new action::ConfirmMove));
  actionMap.insert(pair<Tag, Action*>(Tag::GLOBAL_CONFIRM_MOVE_FRAGMENT, new action::GlobalConfirmMove));
  actionMap.insert(pair<Tag, Action*>(Tag::MOVE_FRAGMENT, new action::Move));
  actionMap.insert(pair<Tag, Action*>(Tag::LOAD, new action::Load));
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

  Arc* arc = new Arc(message.buffer, message.size);
  action->set(arc, message.node);
  delete arc;

  return action;
}

}
}
