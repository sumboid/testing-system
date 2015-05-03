#pragma once
#include "../Action.h"

namespace ts {
namespace system {
namespace action {

class Halt : public ts::system::Action {
public:
  Halt() {}
  ~Halt() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
