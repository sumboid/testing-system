#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"
#include "../../../types/ID.h"

namespace ts {
namespace system {
namespace action {

class GetFragments : public ts::system::Action {
public:
  GetFragments() {}
  ~GetFragments() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
