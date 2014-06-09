#include "Unlock.h"
#include "../../../util/Uberlogger.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Unlock::set(ts::Arc*, ts::NodeID) {
}

void Unlock::set(ts::type::Fragment* f) {
  fragment = f;
}

void Unlock::run() {
  fragmentMgr->unlock(fragment);
}

Action* Unlock::copy() {
  return new Unlock;
}

}
}
}
