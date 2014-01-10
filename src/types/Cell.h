#pragma once

#include <vector>
#include <algorithm>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Particle.h"

namespace ts {
    namespace type {
        class Cell {
            public:
                virtual ~Particle() {}

                virtual void addParticle(Particle* particle) = 0;
                virtual void removeParticl(Particle* particle) = 0;
                virtual void moveParticle(Particle* particle, Cell* cell) = 0;

                virtual int number() = 0;
        };
    }
}
