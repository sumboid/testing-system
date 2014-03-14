#include "ID.h"

namespace ts {
namespace type {

ID::ID(int x, int y, int z) {
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
}}
