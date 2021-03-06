#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"

namespace ts {
namespace system {
namespace action {

class Update : public ts::system::Action {
private:
  ts::type::Fragment* fragment;

public:
  Update(): fragment(0) {}
  ~Update() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
