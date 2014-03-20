#pragma once
#include <cstdlib>
#include "Cell.h"

namespace ts {
namespace type {
class Cell;
class CellTools {
public:
  virtual ~CellTools() {}
  virtual void serialize(Cell* cell, char*& buf, size_t& size) = 0;
  virtual Cell* deserialize(char* buf, size_t size) = 0;
  virtual Cell* createGap(const ID& id) = 0;
};


}}
