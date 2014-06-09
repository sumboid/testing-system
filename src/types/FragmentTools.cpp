#include "FragmentTools.h"
#include "util/FragmentDeserializer.h"
#include "util/FragmentSerializer.h"
#include <cassert>

using ts::Arc;

namespace ts {
namespace type {

using namespace util;

Arc* FragmentTools::boundarySerialize(Fragment* fragment) {
  Arc* arc = new Arc;
  ULOG(error) << "Serialize boundary: " << fragment->id().tostr() << " " << fragment->iteration() << UEND;
  bserialize(fragment, arc);
  FragmentSerializer::id(fragment, arc);
  FragmentSerializer::timestamp(fragment, arc);
  FragmentSerializer::neighbours(fragment, arc);
  return arc;
}

Fragment* FragmentTools::boundaryDeserialize(Arc* arc) {
  Fragment* fragment = bdeserialize(arc);
  FragmentDeserializer::id(fragment, arc);
  FragmentDeserializer::timestamp(fragment, arc);
  FragmentDeserializer::neighbours(fragment, arc);

  return fragment;
}

Arc* FragmentTools::fullSerialize(Fragment* fragment) {
  Arc* arc = new Arc;
  fserialize(fragment, arc);
  FragmentSerializer::id(fragment, arc);
  FragmentSerializer::timestamp(fragment, arc);
  FragmentSerializer::neighboursTimestamp(fragment, arc);
  FragmentSerializer::flags(fragment, arc);
  FragmentSerializer::neighbours(fragment, arc);
  if(fragment->_vlaststateWasSaved) bserialize(fragment->_vlaststate, arc);

  return arc;
}

Fragment* FragmentTools::fullDeserialize(Arc* arc) {
  Fragment* fragment;

  fragment = fdeserialize(arc);
  FragmentDeserializer::id(fragment, arc);
  FragmentDeserializer::timestamp(fragment, arc);
  FragmentDeserializer::neighboursTimestamp(fragment, arc);
  FragmentDeserializer::flags(fragment, arc);
  FragmentDeserializer::neighbours(fragment, arc);
  if(arc->pos() != arc->size()) fragment->_vlaststate = bdeserialize(arc);

  return fragment;
}
}}
