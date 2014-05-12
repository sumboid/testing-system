#pragma once

#include "../Fragment.h"
#include "../../util/Arc.h"
#include "../../system/message/NodeID.h"

namespace ts {
namespace type {
namespace util {
class FragmentDeserializer {
public:
  static void timestamp(ts::type::Fragment* fragment, ts::Arc* arc);
  static void id(ts::type::Fragment* fragment, ts::Arc* arc);
  static void neighbours(ts::type::Fragment* fragment, ts::Arc* arc);
  static void flags(ts::type::Fragment* fragment, ts::Arc* arc);
};
}}}
