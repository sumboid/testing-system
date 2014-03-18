#pragma once
#include <cstdint>

namespace ts {
namespace type {

struct ID {
  enum {X = 0, Y = 1, Z = 2};
  uint64_t c[3];
  ID(uint64_t x = 0, uint64_t y = 0, uint64_t z = 0);
  virtual ~ID();
  bool operator<(const ID&) const;
  bool operator>(const ID&) const;
  bool operator==(const ID& other);
};
}}
