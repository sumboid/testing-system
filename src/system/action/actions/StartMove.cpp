#include "StartMove.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void StartMove::set(char* buffer, size_t, ts::type::NodeID _sender) {
  size_t idsize = 3 * sizeof(uint64_t) / sizeof(char);
  id = ID::deserialize(buffer, idsize);
  node = reinterpret_cast<NodeID*>(buffer + idsize)[0];
  sender = _sender;
}

void StartMove::run()  {
  std::cout << system->id() << ": BEGIN MOVING: " << id.tostr() << std::endl;
  fragmentMgr->updateNeighbours(id, node);
  fragmentMgr->confirmMove(id, sender);
}

Action* StartMove::copy() {
  return new StartMove;
}

}
}
}
