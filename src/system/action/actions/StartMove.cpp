#include "StartMove.h"
#include "../../../util/easylogging++.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void StartMove::set(ts::Arc* arc, ts::type::NodeID _sender) {
  id = ID::deserialize(arc);
  Arc& a = *arc;
  a >> node;
  sender = _sender;
}

void StartMove::run()  {
  LOG(INFO) << "BEGIN MOVING: " << id.tostr();
  fragmentMgr->updateNeighbours(id, node);
  fragmentMgr->confirmMove(id, sender);
}

Action* StartMove::copy() {
  return new StartMove;
}

}
}
}
