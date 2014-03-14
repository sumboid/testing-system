#pragma once
#include "ReduceData.h"
#include <cstdlib>

namespace ts {
namespace type {

class ReduceDataTools {
public:
  virtual ~ReduceDataTools() {}
  virtual void serialize(ReduceData* data, char*& buf, size_t& size) = 0;
  virtual ReduceData* deserialize(void* buf, size_t size) = 0;
  virtual ReduceData* reduce(ReduceData*, ReduceData*) = 0;
};

}}
