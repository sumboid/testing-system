#pragma once

#include "../Cell.h"

namespace ts {
namespace type {
namespace util {

class CellSerializer {
public:
  static size_t timestamp(ts::type::Cell* cell, char*& buf);
  static size_t id(ts::type::Cell* cell, char*& buf);
  static size_t neighbours(ts::type::Cell* cell, char*& buf);
  static size_t flags(ts::type::Cell* cell, char*& buf);

  // Special
  static size_t timestamp(const ts::type::Timestamp&  timestamp, char*& buf);
  static size_t id(const ts::type::ID& id, char*& buf);
};
}}}
