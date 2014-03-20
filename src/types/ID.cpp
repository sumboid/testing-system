#include "ID.h"

namespace ts {
namespace type {

ID::ID(uint64_t x, uint64_t y, uint64_t z) {
  c[X] = x;
  c[Y] = y;
  c[Z] = z;
}

ID::~ID() {}
bool ID::operator<(const ID&) const {
  return true; //XXX: Need to check map behaviour
}
bool ID::operator>(const ID&) const {
  return false; //XXX: Need to check map behaviour
}
bool ID::operator==(const ID& other) {
  return c[X] == other.c[X] &&
         c[Y] == other.c[Y] &&
         c[Z] == other.c[Z];
}

std::string ID::tostr() {
  return "(" + std::to_string(c[X]) + ", " + std::to_string(c[Y]) + ", " + std::to_string(c[Z]) + ")";
}
}}
