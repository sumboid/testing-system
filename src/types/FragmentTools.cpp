#include "FragmentTools.h"
#include "util/FragmentDeserializer.h"
#include "util/FragmentSerializer.h"

using ts::Arc;

namespace ts {
namespace type {

using namespace util;

Arc* FragmentTools::boundarySerialize(Fragment* fragment) {
  Arc* arc = new Arc;
  serialize(fragment, arc);
  FragmentSerializer::id(fragment, arc);
  FragmentSerializer::timestamp(fragment, arc);
  FragmentSerializer::neighbours(fragment, arc);
  return arc;
}

Fragment* FragmentTools::boundaryDeserialize(Arc* arc) {
  Fragment* fragment = deserialize(arc);
  FragmentDeserializer::id(fragment, arc);
  FragmentDeserializer::timestamp(fragment, arc);
  FragmentDeserializer::neighbours(fragment, arc);

  return fragment;
}

Arc* FragmentTools::fullSerialize(Fragment* fragment) {
  Arc* arc = new Arc;
  serialize(fragment, arc);
  FragmentSerializer::id(fragment, arc);
  FragmentSerializer::timestamp(fragment, arc);
  FragmentSerializer::neighboursTimestamp(fragment, arc);
  FragmentSerializer::flags(fragment, arc);
  FragmentSerializer::neighbours(fragment, arc);

  return arc;
}

Fragment* FragmentTools::fullDeserialize(Arc* arc) {
  Fragment* fragment;

  fragment = deserialize(arc);
  ULOG(arc) << "(deserialize) Reading position: " << arc->pos() << UEND;
  FragmentDeserializer::id(fragment, arc);
  ULOG(arc) << "(id) Reading position: " << arc->pos() << UEND;
  FragmentDeserializer::timestamp(fragment, arc);
  ULOG(arc) << "(timestamp) Reading position: " << arc->pos() << UEND;
  FragmentDeserializer::neighboursTimestamp(fragment, arc);
  ULOG(arc) << "(ntimestamp) Reading position: " << arc->pos() << UEND;
  FragmentDeserializer::flags(fragment, arc);
  ULOG(arc) << "(flags) Reading position: " << arc->pos() << UEND;
  FragmentDeserializer::neighbours(fragment, arc);

  return fragment;
}
}}
