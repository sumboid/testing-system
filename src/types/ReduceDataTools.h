#pragma once
#include "ReduceData.h"
#include "../util/Arc.h"
#include <cstdlib>

namespace ts {
namespace type {

class ReduceDataTools {
public:
  virtual ~ReduceDataTools() {}
  virtual Arc* serialize(ReduceData* data) = 0;
  virtual ReduceData* deserialize(ts::Arc* arc) = 0;
  virtual ReduceData* reduce(ReduceData*, ReduceData*) = 0;
};

}}
