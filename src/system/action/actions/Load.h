#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"
#include <stdint.h>

namespace ts {
namespace system {
namespace action {

class Load : public ts::system::Action {
private:
  uint64_t load;
  ts::NodeID node;

public:
  Load(): load(0) {}
  ~Load() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}}}
