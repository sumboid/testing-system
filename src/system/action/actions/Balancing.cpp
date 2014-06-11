#include "Balancing.h"
#include "../../../util/Uberlogger.h"

namespace ts {
namespace system {
namespace action {

void Balancing::set(ts::Arc*, ts::NodeID) {
}

void Balancing::set(std::map<ts::NodeID, double> _amount) {
  amount = _amount;
}

void Balancing::run() {
  fragmentMgr->moveFragment(amount);
}

Action* Balancing::copy() {
  return new Balancing;
}

}
}
}
