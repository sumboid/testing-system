#include "Comm.h"

namespace ts {
  namespace system {

Comm::Comm(int* argc, char ***argv, const int& mode) : max_request_id(0) {
  int provided;
  MPI_Init_thread(argc, argv, mode, &provided);
  if (provided != mode)
    printf("MPI Error : get only %d\n", provided);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  int periods = 0;
  //MPI_Comm_dup(MPI_COMM_WORLD, &comm);
  MPI_Cart_create(MPI_COMM_WORLD, 1, &size, &periods, 0, &comm);
  MPI_Comm_rank(comm, &rank);
}

Comm::~Comm() {
  MPI_Comm_free(&comm);
  MPI_Finalize();
}

void Comm::send(const void *buf, const size_t& buf_size, const unsigned int& tag, const int& dest_rank) {
  MPI_Send((void*)buf, buf_size, MPI_BYTE, dest_rank, tag, comm);
}

unsigned int Comm::isend(const void *buf, const size_t& buf_size, const unsigned int& tag, const int& dest_rank) {
  const unsigned int request_id = max_request_id++;
  MPI_Request req;
  MPI_Isend((void*)buf, buf_size, MPI_BYTE, dest_rank, tag, comm, &req);
  requests[request_id] = req;
  return request_id;
}

void Comm::recv(void *buf, const size_t& buf_size, const unsigned int& tag, const int& src_rank) {
  MPI_Recv(buf, buf_size, MPI_BYTE, src_rank, tag, comm, MPI_STATUS_IGNORE);
}

bool Comm::test(const unsigned int& request_id) {
  int is_finished = 0;
  MPI_Test(&requests[request_id], &is_finished, MPI_STATUSES_IGNORE);
  if (is_finished) {
    requests.erase(request_id);
    return true;
  }
  return false;
}

void Comm::barrier() {
  MPI_Barrier(comm);
}

bool Comm::iprobeAny(size_t& size, unsigned int& tag, int& node) {
  int flag = 0;
  MPI_Status stat;
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &flag, &stat);
  if (flag) {
    int sz;
    MPI_Get_count(&stat, MPI_BYTE, &sz);
    size = sz;
    tag = stat.MPI_TAG;
    node = stat.MPI_SOURCE;
    return true;
  }
  return false;
}

}
}

