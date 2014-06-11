#pragma once
#include "../Action.h"
#include "../../../types/Fragment.h"
#include "../../../types/ID.h"
#include <map>

namespace ts {
namespace system {
namespace action {

class Balancing : public ts::system::Action {
private:
  std::map<ts::NodeID, double> amount;

public:
  Balancing() {}
  ~Balancing() override {}
  void set(ts::Arc* arc, ts::NodeID id) override;
  void set(std::map<ts::NodeID, double> _amount);
  void run() override;

  Action* copy() override;
};

}
}
}
