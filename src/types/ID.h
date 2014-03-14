#pragma once

namespace ts {
namespace type {

struct ID {
  enum {X = 0, Y = 1, Z = 2};
  unsigned int c[3];
  ID(int x = 0, int y = 0, int z = 0);
  virtual ~ID();
  bool operator<(const ID&) const;
  bool operator>(const ID&) const;
  bool operator==(const ID& other);
};
}}
