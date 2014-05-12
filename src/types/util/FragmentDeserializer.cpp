#include "FragmentDeserializer.h"

using ts::NodeID;
namespace ts {
namespace type {
namespace util {

using ts::type::Fragment;

void FragmentDeserializer::timestamp(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  a >> fragment->_viteration;
  a >> fragment->_vprogress;
}

void FragmentDeserializer::id(Fragment* fragment, ts::Arc* arc) {
  fragment->_vid = ID::deserialize(arc);
}

void FragmentDeserializer::neighbours(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  size_t lsize;
  a >> lsize;
  for(uint64_t i = 0; i < lsize; ++i) {
    ID id = ID::deserialize(arc);
    NodeID nid;
    a >> nid;
    fragment->addNeighbour(id, nid);
  }
}
void FragmentDeserializer::flags(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  a >> fragment->_vreduce;
  a >> fragment->_vreduced;
  a >> fragment->_vupdate;
  a >> fragment->_vneighbours;
  a >> fragment->_vend;
}
}}}
