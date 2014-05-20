#pragma once
#include "../Action.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class StartMove : public ts::system::Action {
private:
  ts::type::ID id;
  NodeID node;
  NodeID sender;

public:
  StartMove(): id(ts::type::ID(0,0,0)), node(0), sender(0) {}
  ~StartMove() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
