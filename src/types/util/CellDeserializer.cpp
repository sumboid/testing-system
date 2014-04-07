#include "CellDeserializer.h"
#include <tuple>
#include <vector>


namespace ts {
namespace type {
namespace util {

using std::tuple;
using std::vector;
using ts::type::Cell;

void CellDeserializer::timestamp(Cell* cell, char* buf, size_t) {
  uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
  cell->_viteration = lbuf[0];
  cell->_vprogress = lbuf[1];
}

void CellDeserializer::id(Cell* cell, char* buf, size_t size) {
  cell->_vid = ID::deserialize(buf, size);
}

void CellDeserializer::neighbours(Cell* cell, char* buf, size_t size) {
  uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
  uint64_t lsize = lbuf[0];
  uint64_t counter = 1;
  for(uint64_t i = 0; i < lsize; ++i) {
    ID id = ID(lbuf[counter], lbuf[counter + 1], lbuf[counter + 2]);
    NodeID nid = buf[counter + 3];
    cell->updateNeighbour(id, nid);
    counter += 4;
  }
}
void CellDeserializer::flags(Cell* cell, char* buf, size_t size) {
  bool* lbuf = reinterpret_cast<bool*>(buf);

  cell->_vreduce = lbuf[0];
  cell->_vreduced = lbuf[1];
  cell->_vupdate = lbuf[2];
  cell->_vneighbours = lbuf[3];
  cell->_vend = lbuf[4];
}
}}}
