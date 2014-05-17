#include "ID.h"
#include <iostream>

namespace ts {
namespace type {

ID::ID(uint64_t x, uint64_t y, uint64_t z) {
  c[X] = x;
  c[Y] = y;
  c[Z] = z;
  str = "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
}

ID::~ID() {}

bool ID::operator<(const ID& other) const {
  return str.compare(other.str) < 0;
}
bool ID::operator>(const ID& other) const {
  return str.compare(other.str) > 0;
}

bool ID::operator==(const ID& other) const {
  return str.compare(other.str) == 0;
}

std::string ID::tostr() const {
  return str;
}

void ID::serialize(ts::Arc* arc) const {
  Arc& a = *arc;
  a << c[X];
  a << c[Y];
  a << c[Z];
}

ID ID::deserialize(ts::Arc* arc) {
  Arc& a = *arc;
  ID result;

  a >> result.c[X];
  a >> result.c[Y];
  a >> result.c[Z];

  return result;
}

void ID::operator= (const ID& another) {
  c[X] = another.c[X];
  c[Y] = another.c[Y];
  c[Z] = another.c[Z];
}
}}
