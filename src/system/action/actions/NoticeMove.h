#pragma once
#include "../Action.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class NoticeMove : public ts::system::Action {
private:
  ts::type::ID id;
  NodeID node;
  NodeID sender;

public:
  NoticeMove(): id(ts::type::ID(0,0,0)), node(0), sender(0) {}
  ~NoticeMove() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
