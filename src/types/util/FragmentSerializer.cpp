#include "FragmentSerializer.h"
#include <iostream>
#include <tuple>
namespace ts {
namespace type {
namespace util {

using ts::type::Fragment;
using std::tie;
size_t FragmentSerializer::timestamp(Fragment* fragment, char*& buf) {
  uint64_t* lbuf = new uint64_t[2];
  lbuf[0] = fragment->_viteration;
  lbuf[1] = fragment->_vprogress;

  buf = reinterpret_cast<char*>(lbuf);
  return 2 * sizeof(uint64_t) / sizeof(char);
}

size_t FragmentSerializer::id(Fragment* fragment, char*& buf) {
  ID id = fragment->id();
  return id.serialize(buf);
}

size_t FragmentSerializer::neighbours(Fragment* fragment, char*& buf) {
  size_t size = fragment->_vneighboursLocation.size() * 4 + 1;
  uint64_t* lbuf = new uint64_t[size];
  lbuf[0] = fragment->_vneighboursLocation.size();
  size_t counter = 1;
  for(auto n : fragment->_vneighboursLocation) {
    ID id = n.first;
    lbuf[counter] = id.c[0];
    lbuf[counter + 1] = id.c[1];
    lbuf[counter + 2] = id.c[2];
    lbuf[counter + 3] = n.second;
    counter += 4;
  }

  buf = reinterpret_cast<char*>(lbuf);
  return size * sizeof(uint64_t) / sizeof(char);
}

size_t FragmentSerializer::flags(Fragment* fragment, char*& buf) {
  size_t size = 5;
  bool* lbuf = new bool[size];

  lbuf[0] = fragment->_vreduce;
  lbuf[1] = fragment->_vreduced;
  lbuf[2] = fragment->_vupdate;
  lbuf[3] = fragment->_vneighbours;
  lbuf[4] = fragment->_vend;

  buf = reinterpret_cast<char*>(lbuf);
  return size * sizeof(bool) / sizeof(char);
}

size_t FragmentSerializer::timestamp(const ts::type::Timestamp& timestamp, char*& buf) {
  uint64_t* lbuf = new uint64_t[2];
  tie(lbuf[0], lbuf[1]) = timestamp;
  buf = reinterpret_cast<char*>(lbuf);
  return 2 * sizeof(uint64_t) / sizeof(char);
}
}}}
