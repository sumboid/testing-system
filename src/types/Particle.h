#pragma once

#include "Serializable.h"

namespace ts::type {
    class Particle: public Serializable {
        private:
            double speed;
            double mass;

        public:
            Particle(): speed(0), mass(0) {}
            Particle(int _speed, int _mass): speed(_speed), mass(_mass) {}
            virtual ~Particle() 
