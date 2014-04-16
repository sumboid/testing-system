#include "Move.h"
#include <iostream>

namespace ts {
namespace system {
namespace action {

  void Move::set(char* buffer, size_t size, ts::type::NodeID) {
    fragment = fragmentTools->fullDeserialize(buffer, size);
  }

  void Move::run() {
    std::cout << system->id() << ": MOVING: " << fragment->id().tostr() << std::endl;
    system->addFragment(fragment);
  }

  Action* Move::copy() {
    return new Move;
  }
}}}
