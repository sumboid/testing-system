#pragma once
#include "../Action.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class StartMove : public ts::system::Action {
private:
  ts::type::Fragment* fragment;

public:
  StartMove() {}
  ~StartMove() override {}
  void set(char* buffer, size_t size, ts::type::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
