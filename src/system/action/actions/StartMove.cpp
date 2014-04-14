#include "StartMove.h"

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void StartMove::set(char* buffer, size_t, ts::type::NodeID _sender) {
  size_t idsize = 3 * sizeof(uint64_t) / sizeof(char);
  id = ID::deserialize(buffer, idsize);
  node = reinterpret_cast<int>(buffer + idsize);
  sender = _sender;
}

void StartMove::run()  {
  fragmentMgr->updateNeighbours(id, node);
  fragmentMgr->confirmMove(id, node);
}

Action* StartMove::copy() {
  return new StartMove;
}

}
}
}
