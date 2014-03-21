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

size_t ID::serialize(char*& buf) {
  size_t size = 3 * sizeof(uint64_t);
  uint64_t* lbuf = new uint64_t[3];

  lbuf[0] = c[X];
  lbuf[1] = c[Y];
  lbuf[2] = c[Z];

  buf = reinterpret_cast<char*>(lbuf);

  return size;
}

ID ID::deserialize(char* buf, size_t size) {
  ID result;

  uint64_t* raw = reinterpret_cast<uint64_t*>(buf);
  result.c[0] = raw[0];
  result.c[1] = raw[1];
  result.c[2] = raw[2];

  return result;
}
}}
