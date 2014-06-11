#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class Unlock : public ts::system::Action {
private:
  ts::type::Fragment* fragment;

public:
  Unlock(): fragment(0) {}
  ~Unlock() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void set(ts::type::Fragment* f);
  void run() override;

  Action* copy() override;
};

}
}
}
