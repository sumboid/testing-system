#pragma once

#include <vector>
#include <algorithm>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Particle.h"

namespace ts {
namespace type {
  class AbstractCell {
  public:
    virtual ~Abstract() {}

    virtual void addParticle(Particle* particle) = 0;
    virtual void removeParticle(Particle* particle) = 0;
    virtual int number() = 0;
  };
}}
