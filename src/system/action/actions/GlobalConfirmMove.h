#pragma once
#include "../Action.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class GlobalConfirmMove : public ts::system::Action {
private:
  ts::type::ID id;
  NodeID sender;

public:
  GlobalConfirmMove(): id(ts::type::ID(0,0,0)), sender(0) {}
  ~GlobalConfirmMove() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
