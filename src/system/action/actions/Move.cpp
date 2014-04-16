#include "Move.h"

namespace ts {
namespace system {
namespace action {

  void Move::set(char* buffer, size_t size, ts::type::NodeID) {
    fragment = fragmentTools->fullDeserialize(buffer, size);
  }

  void Move::run() {
    system->addFragment(fragment);
  }

  Action* Move::copy() {
    return new Move;
  }
}}}
