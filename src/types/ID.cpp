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
  return tostr().compare(other.tostr()) < 0;
}
bool ID::operator>(const ID& other) const {
  return tostr().compare(other.tostr()) > 0;
}

bool ID::operator==(const ID& other) const {
  return tostr().compare(other.tostr()) == 0;
}

std::string ID::tostr() const {
  return std::to_string(c[X]) + " " + std::to_string(c[Y]) + " " + std::to_string(c[Z]);
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

ID& ID::operator= (const ID& another) {
  c[X] = another.c[X];
  c[Y] = another.c[Y];
  c[Z] = another.c[Z];
  return *this;
}
}}
