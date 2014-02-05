#include <chrono>
#include "MessageMgr.h"

using std::thread;
using std::this_thread;
using std::chrono;

ts::system::MessageMgr(): end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
}

void ts::system::run() {
  sender = thread(&MessageMgr::sendLoop, this);
  receiver = thread(&MessageMgr::receiverLoop, this);
}

void ts::system::receiverLoop() {
  while(true) {
    size_t size;
    unsigned int tag;
    int node;
    if(comm->iprobeAny(&size, &tag, &node)) {
      void* buffer = new void[size];
      comm.recv(buffer, size, tag, node);
      switch(tag) {
        case UPDATE_CELL:
          auto cell = AbstractCellTools().deserialize(buffer, size);
          cellMgr->updateExternalCell(cell);
          break;
        default:
          break;
      }
    }
    else {
      if(end) return;
      this_thread::sleep_for(chrono::seconds(1));
    }
  }
}

void ts::system::senderLoop() {
  while(true) {
    if(end) return;
    if(sendQueue.empty()) {
      this_thread::sleep_for(chrono::seconds(1));
      continue;
    }
    auto message = sendQueue.pop_front();
    comm->send(message.buffer, message.size, message.tag, message.node);
    delete message.buffer;
  }
}

