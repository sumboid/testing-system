#include "GlobalConfirmMove.h"
#include "../../../util/Uberlogger.h"

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void GlobalConfirmMove::set(ts::Arc* arc, ts::NodeID _sender) {
  id = ID::deserialize(arc);
  sender = _sender;
}

void GlobalConfirmMove::run()  {
  ULOG(move) << "Move was globally confirmed: " << id.tostr() << UEND;
  fragmentMgr->moveFragmentGlobalAccept(id, sender);
}

Action* GlobalConfirmMove::copy() {
  return new GlobalConfirmMove;
}

}
}
}

