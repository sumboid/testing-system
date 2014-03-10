#pragma once

#include "../types/AbstractCell.h"

namespace ts {
namespace distr {

/**
 * @brief The Distributor class
 * This abstraction need to incapsulate core of system (system namespace).
 * It need to create primary distribution of cells and provide high-level
 * methods to add cells.
 */

class Distributor {
private:
  size_t nodesNumber;
  size_t rank;
public:
  Distributor(size_t rank, size_t size);
  ~Distributor();
};

}
}
