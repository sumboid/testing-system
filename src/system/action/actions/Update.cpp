#include "Update.h"
namespace ts {
namespace system {
namespace action {

void Update::set(char* buffer, size_t size, ts::type::NodeID) {
  cell = cellTools->boundaryDeserialize(buffer, size);
}

void Update::run() {
  cellMgr->updateExternalCell(cell);
}

Action* Update::copy() {
  return new Update;
}

}
}
}
