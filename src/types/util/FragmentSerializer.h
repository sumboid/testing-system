#pragma once

#include "../Fragment.h"

namespace ts {
namespace type {
namespace util {

class FragmentSerializer {
public:
  static size_t timestamp(ts::type::Fragment* fragment, char*& buf);
  static size_t id(ts::type::Fragment* fragment, char*& buf);
  static size_t neighbours(ts::type::Fragment* fragment, char*& buf);
  static size_t flags(ts::type::Fragment* fragment, char*& buf);

  // Special
  static size_t timestamp(const ts::type::Timestamp&  timestamp, char*& buf);
  static size_t id(const ts::type::ID& id, char*& buf);
};
}}}
