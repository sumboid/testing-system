#pragma once
#include <cstdlib>
#include "AbstractCell.h"

namespace ts {
namespace type {

class AbstractCellTools {
public:
  virtual ~AbstractCellTools() {}
  virtual void serialize(AbstractCell* cell, char*& buf, size_t& size) = 0;
  virtual AbstractCell* deserialize(char* buf, size_t size) = 0;
};


}}
