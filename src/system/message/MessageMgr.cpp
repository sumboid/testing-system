#include <iostream>
#include <chrono>
#include <cassert>
#include "MessageMgr.h"

using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using ts::type::AbstractCell;
using ts::type::AbstractCellTools;
using ts::type::ReduceDataTools;
using ts::type::ReduceData;

ts::system::MessageMgr::MessageMgr(): end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
  _size = comm->getSize();
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
        case REDUCE_DATA:
          sys->putReduceData(reduceTool->deserialize(buffer, size));
          break;
        default:
          break;
      }
      delete[] buffer;
    }
    else {
      if(end.load()) return;
      sleep_for(seconds(1));
    }
  }
}

void ts::system::MessageMgr::sendLoop() {
  while(true) {
    if(end.load()) return;
    queueMutex.lock();
    if(sendQueue.empty()) {
      queueMutex.unlock();
      sleep_for(seconds(1));
      continue;
    }
    auto message = sendQueue.front();
    sendQueue.pop();

    queueMutex.unlock();
    comm->send(message->buffer, message->size, message->tag, message->node);
    delete[] message->buffer;
    delete message;
  }
}

void ts::system::MessageMgr::send(NodeID node, Tag tag, AbstractCell* cell) {
  Message* message = new Message;
  cellTool->serialize(cell, message->buffer, message->size);
  message->tag = tag;
  message->node = node;
  queueMutex.lock();
  sendQueue.push(message);
  queueMutex.unlock();
}

void ts::system::MessageMgr::send(ReduceData* reduceData) {
  queueMutex.lock();
  for(size_t i = 0; i < _size; ++i) {
    if(i != id) {
      Message* message = new Message;
      reduceTool->serialize(reduceData, message->buffer, message->size);
      message->tag = REDUCE_DATA;
      message->node = i;
      sendQueue.push(message);
    }
  }
  queueMutex.unlock();
}

void ts::system::MessageMgr::stop() {
  end.store(true);
}
