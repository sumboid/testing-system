#include "Update.h"
#include "../../../util/Uberlogger.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Update::set(ts::Arc* arc, ts::NodeID) {
  fragment = fragmentTools->boundaryDeserialize(arc);
}

void Update::run() {
  UBERLOG() << "BOUNDARY UPDATED: " << fragment->id().tostr() << UBEREND();
  fragmentMgr->updateExternalFragment(fragment);
}

Action* Update::copy() {
  return new Update;
}

}
}
}
