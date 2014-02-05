#pragma once

#include <mpi.h>
#include <map>

namespace ts {
  namespace system {

/**
 * Implementation of communicator with usage of MPI
 */
class Comm {
private:
  int rank;
  int size;
  MPI_Comm comm;
  std::map<unsigned int, MPI_Request> requests;
  unsigned int max_request_id;

public:
  Comm(int* argc, char ***argv, const int& mode = MPI_THREAD_SINGLE);
  ~Comm();

  void send(const void *buf, const size_t& buf_size, const unsigned int& tag, const int& dst_rank);
  unsigned int isend(const void *buf, const size_t& buf_size, const unsigned int& tag, const int& dst_rank);

  void recv(void *buf, const size_t& buf_size, const unsigned int& tag, const int& src_rank);

  bool iprobeAny(size_t& size, unsigned int& tag, int& node);

  bool test(const unsigned int& request_id);

  void barrier();

  int getRank() const {
    return rank;
  }

  int getSize() const {
    return size;
  }
};

}
}
