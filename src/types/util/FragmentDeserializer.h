#pragma once

#include "../Fragment.h"

namespace ts {
namespace type {
namespace util {
class FragmentDeserializer {
public:
  static void timestamp(ts::type::Fragment* fragment, char* buf, size_t size);
  static void id(ts::type::Fragment* fragment, char* buf, size_t size);
  static void neighbours(ts::type::Fragment* fragment, char* buf, size_t size);
  static void flags(ts::type::Fragment* fragment, char* buf, size_t size);
};
}}}
