#include "GetFragment.h"

namespace ts {
namespace system {
namespace action {

  void GetFragment::set(char* buffer, size_t size, ts::type::NodeID) {
    fragment = fragmentTools->fullDeserialize(buffer, size);
  }

  void GetFragment::run() {
    system->addFragment(fragment);
  }

  Action* GetFragment::copy() {
    return new GetFragment;
  }
}}}
