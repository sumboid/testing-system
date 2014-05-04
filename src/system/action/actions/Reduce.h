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
  void set(ts::Arc* arc, ts::type::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
