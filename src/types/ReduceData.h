#pragma once

namespace ts {
namespace type {

class ReduceData {
public:
  virtual ~ReduceData() {}
  virtual ReduceData* copy() = 0;
};

}}
