#include "Move.h"
#include "../../../util/Uberlogger.h"
#include <iostream>

namespace ts {
namespace system {
namespace action {

  void Move::set(ts::Arc* arc, ts::NodeID) {
    fragment = fragmentTools->fullDeserialize(arc);
  }

  void Move::run() {
    ULOG(move) << "Moved: " << fragment->id().tostr() << UBEREND();
    system->addFragment(fragment);
  }

  Action* Move::copy() {
    return new Move;
  }
}}}
