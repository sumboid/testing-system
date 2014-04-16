#include "ConfirmMove.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void ConfirmMove::set(char* buffer, size_t, ts::type::NodeID _sender) {
  size_t idsize = 3 * sizeof(uint64_t) / sizeof(char);
  id = ID::deserialize(buffer, idsize);
  sender = _sender;
}

void ConfirmMove::run()  {
  std::cout << system->id() << ": CONFIRM MOVING: " << id.tostr() << std::endl;
  fragmentMgr->moveFragmentAccept(id, sender);
}

Action* ConfirmMove::copy() {
  return new ConfirmMove;
}

}
}
}

