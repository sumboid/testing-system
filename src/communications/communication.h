#include <mpi.h>

namespace ts::comm {
    class Communicator {
        private:
            enum Tags { PARTICLE, CELL };
            MPI::Comm communicator;
        public: 
            Communicator(): communicator(MPI::COMM_WORLD) {}
            ~Communicator() { MPI::Finalize(); }

            void send(int rank, int tag);
    };
}
