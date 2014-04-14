#include "FragmentDeserializer.h"
#include <tuple>
#include <vector>


namespace ts {
namespace type {
namespace util {

using std::tuple;
using std::vector;
using ts::type::Fragment;

void FragmentDeserializer::timestamp(Fragment* fragment, char* buf, size_t) {
  uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
  fragment->_viteration = lbuf[0];
  fragment->_vprogress = lbuf[1];
}

void FragmentDeserializer::id(Fragment* fragment, char* buf, size_t size) {
  fragment->_vid = ID::deserialize(buf, size);
}

void FragmentDeserializer::neighbours(Fragment* fragment, char* buf, size_t size) {
  uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
  uint64_t lsize = lbuf[0];
  uint64_t counter = 1;
  for(uint64_t i = 0; i < lsize; ++i) {
    ID id = ID(lbuf[counter], lbuf[counter + 1], lbuf[counter + 2]);
    NodeID nid = buf[counter + 3];
    fragment->updateNeighbour(id, nid);
    counter += 4;
  }
}
void FragmentDeserializer::flags(Fragment* fragment, char* buf, size_t size) {
  bool* lbuf = reinterpret_cast<bool*>(buf);

  fragment->_vreduce = lbuf[0];
  fragment->_vreduced = lbuf[1];
  fragment->_vupdate = lbuf[2];
  fragment->_vneighbours = lbuf[3];
  fragment->_vend = lbuf[4];
}
}}}
