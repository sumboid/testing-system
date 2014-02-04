#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "AbstractCell.h"

namespace ts {
namespace type {
  class AbstractParticle {
  public:
    virtual ~Particle() {}
    virtual void push(Cell* cell) = 0;
    virtual void force(Cell* cell) = 0;
  };
}}
