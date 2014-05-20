#include "Reduce.h"
#include "../../../util/Uberlogger.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Reduce::set(ts::Arc* arc, ts::NodeID) {
  data = reduceDataTools->deserialize(arc);
}

void Reduce::run() {
  //UBERLOG() << "REDUCE ACTION!" << UBEREND();
  system->putReduceData(data);
}

Action* Reduce::copy() {
  return new Reduce;
}

}
}
}
