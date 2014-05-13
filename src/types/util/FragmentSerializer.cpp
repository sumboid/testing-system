#include "FragmentSerializer.h"
#include <iostream>
#include <tuple>

using ts::NodeID;
namespace ts {
namespace type {
namespace util {

using ts::type::Fragment;
using std::tie;
void FragmentSerializer::timestamp(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  a << fragment->_viteration;
  a << fragment->_vprogress;
}

void FragmentSerializer::neighboursTimestamp(Fragment* fragment, ts::Arc* arc) {
  timestamp(fragment->_vneighboursState, arc);
}

void FragmentSerializer::id(Fragment* fragment, ts::Arc* arc) {
  fragment->id().serialize(arc);
}

void FragmentSerializer::neighbours(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  a << static_cast<size_t>(fragment->_vneighboursLocation.size());

  for(auto n : fragment->_vneighboursLocation) {
    n.first.serialize(arc);
    a << n.second;
  }
}

void FragmentSerializer::flags(Fragment* fragment, ts::Arc* arc) {
  Arc& a = *arc;
  a << fragment->_vreduce;
  a << fragment->_vreduced;
  a << fragment->_vupdate;
  a << fragment->_vneighbours;
  a << fragment->_vend;
}

void FragmentSerializer::timestamp(const ts::type::Timestamp& timestamp, ts::Arc* arc) {
  Arc& a = *arc;
  a << std::get<0>(timestamp) << std::get<1>(timestamp);
}
}}}
