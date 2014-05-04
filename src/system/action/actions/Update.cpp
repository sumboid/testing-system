#include "Update.h"
#include "../../../util/easylogging++.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Update::set(ts::Arc* arc, ts::type::NodeID) {
  fragment = fragmentTools->boundaryDeserialize(arc);
}

void Update::run() {
  LOG(INFO) << "BOUNDARY UPDATED: " << fragment->id().tostr();
  fragmentMgr->updateExternalFragment(fragment);
}

Action* Update::copy() {
  return new Update;
}

}
}
}
