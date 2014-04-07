#include "Reduce.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void Reduce::set(char* buffer, size_t size, ts::type::NodeID) {
  data = reduceDataTools->deserialize(buffer, size);
}

void Reduce::run() {
  std::cout << "REDUCE ACTION!" << std::endl;
  system->putReduceData(data);
}

Action* Reduce::copy() {
  return new Reduce;
}

}
}
}
