#include "GetFragments.h"
#include "../../../util/Uberlogger.h"
#include <iostream>
namespace ts {
namespace system {
namespace action {

void GetFragments::set(ts::Arc*, ts::NodeID) {
}

void GetFragments::run() {
  auto fs = fragmentMgr->getFragments(359);
  if(!fs.empty()) execMgr->add(fs);
}

Action* GetFragments::copy() {
  return new GetFragments;
}

}
}
}
