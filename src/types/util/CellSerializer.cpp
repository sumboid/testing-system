#include "CellSerializer.h"
#include <tuple>
namespace ts {
namespace type {
namespace util {

using ts::type::Cell;
using std::tie;
size_t CellSerializer::timestamp(Cell* cell, char*& buf) {
  uint64_t* lbuf = new uint64_t[2];
  lbuf[0] = cell->_viteration;
  lbuf[1] = cell->_vprogress;

  buf = reinterpret_cast<char*>(lbuf);
  return 2 * sizeof(uint64_t) / sizeof(char);
}

size_t CellSerializer::id(Cell* cell, char*& buf) {
  ID id = cell->id();
  return id.serialize(buf);
}

size_t CellSerializer::neighbours(Cell* cell, char*& buf) {
  size_t size = cell->_vneighboursLocation.size() * 4 + 1;
  uint64_t* lbuf = new uint64_t[size];
  lbuf[0] = cell->_vneighboursLocation.size();
  size_t counter = 1;
  for(auto n : cell->_vneighboursLocation) {
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

size_t CellSerializer::flags(Cell* cell, char*& buf) {
  size_t size = 5;
  bool* lbuf = new bool[size];

  lbuf[0] = cell->_vreduce;
  lbuf[1] = cell->_vreduced;
  lbuf[2] = cell->_vupdate;
  lbuf[3] = cell->_vneighbours;
  lbuf[4] = cell->_vend;

  buf = reinterpret_cast<char*>(lbuf);
  return size * sizeof(bool) / sizeof(char);
}

size_t CellSerializer::timestamp(const ts::type::Timestamp& timestamp, char*& buf) {
  uint64_t* lbuf = new uint64_t[2];
  tie(lbuf[0], lbuf[1]) = timestamp;
  buf = reinterpret_cast<char*>(lbuf);
  return 2 * sizeof(uint64_t) / sizeof(char);
}
}}}
