#pragma once

#include "../Cell.h"

namespace ts {
namespace type {
namespace util {
class CellDeserializer {
public:
  static void timestamp(ts::type::Cell* cell, char* buf, size_t size);
  static void id(ts::type::Cell* cell, char* buf, size_t size);
  static void neighbours(ts::type::Cell* cell, char* buf, size_t size);
  static void flags(ts::type::Cell* cell, char* buf, size_t size);
};
}}}
