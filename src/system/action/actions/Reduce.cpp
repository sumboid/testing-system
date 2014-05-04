#include "Reduce.h"
#include "../../../util/easylogging++.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Reduce::set(ts::Arc* arc, ts::type::NodeID) {
  data = reduceDataTools->deserialize(arc);
}

void Reduce::run() {
  LOG(INFO) << "REDUCE ACTION!";
  system->putReduceData(data);
}

Action* Reduce::copy() {
  return new Reduce;
}

}
}
}
