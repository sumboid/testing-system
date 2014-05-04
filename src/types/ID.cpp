#include "ID.h"
#include <iostream>

namespace ts {
namespace type {

ID::ID(uint64_t x, uint64_t y, uint64_t z) {
  c[X] = x;
  c[Y] = y;
  c[Z] = z;
}

ID::~ID() {}

bool ID::operator<(const ID& other) const {
  return c[X] > other.c[X] ||
         c[Y] > other.c[Y] ||
         c[Z] > other.c[Z];
}
bool ID::operator>(const ID& other) const {
  return c[X] < other.c[X] ||
         c[Y] < other.c[Y] ||
         c[Z] < other.c[Z];
}

bool ID::operator==(const ID& other) const {
  return c[X] == other.c[X] &&
         c[Y] == other.c[Y] &&
         c[Z] == other.c[Z];
}

std::string ID::tostr() const {
  return "(" + std::to_string(c[X]) + ", " + std::to_string(c[Y]) + ", " + std::to_string(c[Z]) + ")";
}

void ID::serialize(ts::Arc* arc) const {
  Arc& a = *arc;
  a << c[X] << c[Y] << c[Z];
}

ID ID::deserialize(ts::Arc* arc) {
  Arc& a = *arc;
  ID result;

  a >> result.c[0];
  a >> result.c[1];
  a >> result.c[2];

  return result;
}
}}
