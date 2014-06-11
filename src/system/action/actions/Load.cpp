#include "Load.h"
#include "../../../util/Uberlogger.h"
#include <iostream>

namespace ts {
namespace system {
namespace action {

  void Load::set(ts::Arc* arc, ts::NodeID id) {
    ts::Arc& a = *arc;
    a >> load;
    node = id;
  }

  void Load::run() {
    system->loadChange(node, load);
  }

  Action* Load::copy() {
    return new Load;
  }
}}}
