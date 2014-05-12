#pragma once
#include <cstdlib>
#include "Fragment.h"
#include "../util/Arc.h"

namespace ts {
namespace type {
class Fragment;
class FragmentTools {
public:
  virtual ~FragmentTools() {}
  virtual void serialize(Fragment* fragment, ts::Arc* arc) = 0;
  virtual Fragment* deserialize(ts::Arc* arc) = 0;
  virtual Fragment* createGap(const ID& id) = 0;

  ts::Arc* fullSerialize(Fragment* fragment);
  ts::Arc* boundarySerialize(Fragment* fragment);

  Fragment* fullDeserialize(ts::Arc* arc);
  Fragment* boundaryDeserialize(ts::Arc* arc);
};



}}
