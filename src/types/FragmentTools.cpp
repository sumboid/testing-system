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
  return arc;
}

Fragment* FragmentTools::boundaryDeserialize(Arc* arc) {
  Fragment* fragment = deserialize(arc);
  FragmentDeserializer::id(fragment, arc);
  FragmentDeserializer::timestamp(fragment, arc);

  return fragment;
}

Arc* FragmentTools::fullSerialize(Fragment* fragment) {
  Arc* arc = new Arc;
  serialize(fragment, arc);
  FragmentSerializer::id(fragment, arc);
  FragmentSerializer::timestamp(fragment, arc);
  FragmentSerializer::neighbours(fragment, arc);
  FragmentSerializer::flags(fragment, arc);

  return arc;
}

Fragment* FragmentTools::fullDeserialize(Arc* arc) {
  Fragment* fragment;

  fragment = deserialize(arc);
  FragmentDeserializer::id(fragment, arc);
  FragmentDeserializer::timestamp(fragment, arc);
  FragmentDeserializer::neighbours(fragment, arc);
  FragmentDeserializer::flags(fragment, arc);

  return fragment;
}
}}
