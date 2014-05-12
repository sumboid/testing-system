#pragma once

#include "../Fragment.h"
#include "../../util/Arc.h"

namespace ts {
namespace type {
namespace util {

class FragmentSerializer {
public:
  static void timestamp(ts::type::Fragment* fragment, ts::Arc* arc);
  static void id(ts::type::Fragment* fragment, ts::Arc* arc);
  static void neighbours(ts::type::Fragment* fragment, ts::Arc* arc);
  static void flags(ts::type::Fragment* fragment, ts::Arc* arc);

  // Special
  static void timestamp(const ts::type::Timestamp&  timestamp, ts::Arc* arc);
  static void id(const ts::type::ID& id, ts::Arc* arc);
};
}}}
