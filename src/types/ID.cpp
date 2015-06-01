#include "ID.h"
#include <iostream>

namespace ts {
namespace type {

ID::ID(uint64_t x, uint64_t y, uint64_t z, uint64_t v) {
  c[X] = x;
  c[Y] = y;
  c[Z] = z;
  c[V] = v;
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

bool ID::vcompare(const ID &other) const {
  return c[X] == other.c[X] && c[Y] == other.c[Y] && c[Z] == other.c[Z];
}

std::string ID::tostr() const {
  return std::to_string(c[X]) + " " + std::to_string(c[Y]) + " " + std::to_string(c[Z]) + " " + std::to_string(c[V]);
}

void ID::serialize(ts::Arc* arc) const {
  Arc& a = *arc;
  a << c[X];
  a << c[Y];
  a << c[Z];
  a << c[V];
}

ID ID::deserialize(ts::Arc* arc) {
  Arc& a = *arc;
  ID result;

  a >> result.c[X];
  a >> result.c[Y];
  a >> result.c[Z];
  a >> result.c[V];
  return result;
}

ID& ID::operator= (const ID& another) {
  c[X] = another.c[X];
  c[Y] = another.c[Y];
  c[Z] = another.c[Z];
  c[V] = another.c[V];
  return *this;
}
}}
