#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"

namespace ts {
namespace system {
namespace action {

class Load : public ts::system::Action {
private:
  int load;
  ts::NodeID node;

public:
  Load(): load(0) {}
  ~Load() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}}}
