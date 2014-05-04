#include "Move.h"
#include "../../../util/easylogging++.h"
#include <iostream>

namespace ts {
namespace system {
namespace action {

  void Move::set(ts::Arc* arc, ts::type::NodeID) {
    fragment = fragmentTools->fullDeserialize(arc);
  }

  void Move::run() {
    LOG(INFO) << "MOVING: " << fragment->id().tostr();
    system->addFragment(fragment);
  }

  Action* Move::copy() {
    return new Move;
  }
}}}
