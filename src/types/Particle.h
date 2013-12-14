#pragma once

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

namespace ts {
    namespace type {
        class Particle {
            private:
                double speed;
                double mass;

                friend class boost::serialization::access;
                template<class Archive>
                    void serialize(Archive & ar, const unsigned int version) {
                        ar & speed & mass;
                    }

            public:
                Particle(): speed(0), mass(0) {}
                Particle(int _speed, int _mass): speed(_speed), mass(_mass) {}
                virtual ~Particle() {}

                void setSpeed(double _speed) { speed = _speed; }
                void setSpeed(double _mass) { mass = _mass; }
                double getSpeed() { return speed; }
                double getMass() { return mass; }
        };
    }
}
