#include <chrono>
#include "MessageMgr.h"

using std::thread;
using std::this_thread;
using std::chrono;
using ts::types::AbstractCell;
using ts::types::NodeID;
using ts::types::AbstractCellTools;

ts::system::MessageMgr::MessageMgr(): end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
}

void ts::system::MessageMgr::run() {
  sender = thread(&MessageMgr::sendLoop, this);
  receiver = thread(&MessageMgr::receiverLoop, this);
}

void ts::system::MessageMgr::join() {
  sender.join();
  receiver.join();
}

void ts::system::MessageMgr::receiverLoop() {
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

void ts::system::MessageMgr::senderLoop() {
  while(true) {
    if(end) return;
    queueMutex.lock();
    if(sendQueue.empty()) {
      queueMutex.unlock();
      this_thread::sleep_for(chrono::seconds(1));
      continue;
    }
    auto message = sendQueue.pop_front();
    queueMutex.unlock();
    comm->send(message.buffer, message.size, message.tag, message.node);
    delete message.buffer;
  }
}

void ts::system::MessageMgr::send(NodeID node, Tag tag, AbstractCell* cell) {
  Message message;
  AbstractCellTools().serialize(cell, message.buffer, message.size);
  message.tag = tag;
  queueMutex.lock();
  sendQueue.push_back(message);
  queueMutex.unlock();
}

void ts::system::MessageMgr::stop() {
  end = true;
}
