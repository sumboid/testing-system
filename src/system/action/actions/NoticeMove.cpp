#include "NoticeMove.h"
#include "../../../util/Uberlogger.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void NoticeMove::set(ts::Arc* arc, ts::NodeID _sender) {
  id = ID::deserialize(arc);
  Arc& a = *arc;
  a >> node;
  sender = _sender;
}

void NoticeMove::run()  {
  fragmentMgr->updateNeighbours(id, node);
  fragmentMgr->globalConfirmMove(id, sender);
}

Action* NoticeMove::copy() {
  return new NoticeMove;
}

}
}
}
