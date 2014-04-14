#pragma once
#include "../Action.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class ConfirmMove : public ts::system::Action {
private:
  ts::type::ID id;
  NodeID sender;

public:
  ConfirmMove() {}
  ~ConfirmMove() override {}
  void set(char* buffer, size_t size, ts::type::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
