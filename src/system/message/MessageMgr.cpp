#include <chrono>
#include "MessageMgr.h"

using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using ts::type::AbstractCell;
using ts::type::NodeID;
using ts::type::AbstractCellTools;

ts::system::MessageMgr::MessageMgr(): end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
}

void ts::system::MessageMgr::run() {
  sender = thread(&MessageMgr::sendLoop, this);
  receiver = thread(&MessageMgr::receiveLoop, this);
}

void ts::system::MessageMgr::join() {
  sender.join();
  receiver.join();
}

void ts::system::MessageMgr::receiveLoop() {
  while(true) {
    size_t size;
    unsigned int tag;
    int node;
    if(comm->iprobeAny(size, tag, node)) {
      char* buffer = new char[size];
      comm->recv(buffer, size, tag, node);
      switch(tag) {
        case UPDATE_CELL:
          cellMgr->updateExternalCell(cellTool->deserialize(buffer, size));
          break;
        default:
          break;
      }
    }
    else {
      if(end) return;
      sleep_for(seconds(1));
    }
  }
}

void ts::system::MessageMgr::sendLoop() {
  while(true) {
    if(end) return;
    queueMutex.lock();
    if(sendQueue.empty()) {
      queueMutex.unlock();
      sleep_for(seconds(1));
      continue;
    }
    auto message = sendQueue.front();
    queueMutex.unlock();
    comm->send(message.buffer, message.size, message.tag, message.node);
    delete message.buffer;
  }
}

void ts::system::MessageMgr::send(NodeID node, Tag tag, AbstractCell* cell) {
  Message message;
  cellTool->serialize(cell, message.buffer, message.size);
  message.tag = tag;
  queueMutex.lock();
  sendQueue.push(message);
  queueMutex.unlock();
}

void ts::system::MessageMgr::stop() {
  end = true;
}
