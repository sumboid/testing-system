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
  Update() {}
  ~Update() override {}
  void set(char* buffer, size_t size, ts::type::NodeID id) override;
  void run() override;

  Action* copy() override;
};

}
}
}
