#include "Halt.h"
#include "../../../util/Uberlogger.h"
#include <iostream>

using ts::type::ID;
namespace ts {
namespace system {
namespace action {

void Halt::set(ts::Arc*, ts::NodeID) {}

void Halt::run()  {
  system->end();
}

Action* Halt::copy() {
  return new Halt;
}

}
}
}
