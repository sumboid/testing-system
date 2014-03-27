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
  void set(char* buffer, size_t size, ts::type::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
