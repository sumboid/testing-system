#pragma once
#include <cstdint>
#include <string>
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
  std::string tostr();

  size_t serialize(char*& buf);
  static ID deserialize(char* buf, size_t size);
};
}}
