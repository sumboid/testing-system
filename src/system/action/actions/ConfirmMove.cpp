#include "ConfirmMove.h"
#include "../../../util/Uberlogger.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void ConfirmMove::set(ts::Arc* arc, ts::NodeID _sender) {
  id = ID::deserialize(arc);
  sender = _sender;
}

void ConfirmMove::run()  {
  fragmentMgr->moveFragmentAccept(id);
}

Action* ConfirmMove::copy() {
  return new ConfirmMove;
}

}
}
}

