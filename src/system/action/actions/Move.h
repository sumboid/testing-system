#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"

namespace ts {
namespace system {
namespace action {

class Move : public ts::system::Action {
private:
  ts::type::Fragment* fragment;

public:
  Move(): fragment(0) {}
  ~Move() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}}}
