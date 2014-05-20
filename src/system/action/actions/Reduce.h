#pragma once
#include "../Action.h"
#include "../../../types/ReduceData.h"

namespace ts {
namespace system {
namespace action {

class Reduce : public ts::system::Action {
private:
  ts::type::ReduceData* data;

public:
  Reduce(): data(0) {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
