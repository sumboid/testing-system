#pragma once

#include <vector>
#include <algorithm>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Particle.h"

namespace ts {
    namespace type {
        typedef std::vector<Particle>::iterator pi_t;
        class Cell {
            private:
                size_t number;
                double force[8];
                double density;
                std::vector<Particle*> particles;

                friend class boost::serialization::access;
                template<class Archive>
                    void serialize(Archive & ar, const unsigned int version) {
                        ar & number & force & mass & particles;
                    }

            public:
                Particle(size_t _number = 0): number(_number), destiny(0) { for(int i = 0, i < 8; ++i) force[i] = 0; } 
                virtual ~Particle() {}

                void setNumber(size_t _number) { number = _number; }
                void setDensity(double _dencity) { density = _density; }
                void setForce(double* _force) { for(int i = 0; i < 8; ++i) force[i] = _force[i]; }

                pi_t findParticle(Particle* particle) { return find(particles.begin(), particles.end()); }
                void addParticle(Particle* particle) { particles.push_back(particle); }
                void removeParticle(pi_t i) { particles.erase(i, i + 1); }
                void removeParticle(Particle* particle) {
                    pi_t i = findParticle(particle);
                    particles.erase(i, i + 1);
                }
                void moveParticle(Particle* particle, Cell* cell) {
                    pi_d it = findParticle(particle);
                    cell.addParticle(*it);
                    removeParticle(it);
                }

        };
    }
}
