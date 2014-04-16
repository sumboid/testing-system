#include "Update.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Update::set(char* buffer, size_t size, ts::type::NodeID) {
  fragment = fragmentTools->boundaryDeserialize(buffer, size);
}

void Update::run() {
  std::cout << system->id() << ": BOUNDARY UPDATED: " << fragment->id().tostr() << std::endl;
  fragmentMgr->updateExternalFragment(fragment);
}

Action* Update::copy() {
  return new Update;
}

}
}
}
