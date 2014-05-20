#pragma once
#include <cstdint>
#include <string>
#include "../util/Arc.h"

namespace ts {
namespace type {

struct ID {
  enum {X = 0, Y = 1, Z = 2};
  uint64_t c[3];
  ID(uint64_t x = 0, uint64_t y = 0, uint64_t z = 0);
  virtual ~ID();
  bool operator<(const ID&) const;
  bool operator>(const ID&) const;
  bool operator==(const ID& other) const;
  ID& operator= (const ID& another);
  std::string tostr() const;

  void serialize(ts::Arc* arc) const;
  static ID deserialize(ts::Arc* arc);
};
}}
