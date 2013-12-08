#pragma once

#include <mpi.h>

namespace ts::types {
    enum { PARTICLE_ID, CELL_ID };

    class Serializable {
        private:
            size_t size;
            int id;

            char* buffer;
            size_t position;

        public:
            Serializable(int _id, size_t _size): id(_id), size(_size + sizeof(int), position(0) {
                buffer = new char[size];
                MPI::Pack(
            virtual ~Serializable() = 0;
            virtual void* serialize() = 0;
    };
}
