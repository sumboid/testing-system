#include <iostream>
#include <chrono>
#include <cassert>
#include "MessageMgr.h"

using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using ts::type::Fragment;
using ts::type::FragmentTools;
using ts::type::ReduceDataTools;
using ts::type::ReduceData;

namespace ts {
namespace system {

MessageMgr::MessageMgr(): end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
  _size = comm->getSize();
}

MessageMgr::~MessageMgr() {
  delete comm;
}

void MessageMgr::run() {
  sender = thread(&MessageMgr::sendLoop, this);
  receiver = thread(&MessageMgr::receiveLoop, this);
}

void MessageMgr::join() {
  sender.join();
  receiver.join();
}

void MessageMgr::receiveLoop() {
  while(true) {
    size_t size;
    unsigned int tag;
    int node;
    if(comm->iprobeAny(size, tag, node)) {
      char* buffer = new char[size];
      comm->recv(buffer, size, tag, node);
      Message message;
      message.tag = (Tag)tag;
      message.node = node;
      message.buffer = buffer;
      message.size = size;

      Action* action = actionBuilder->build(message);
      sys->addAction(action);
    }
    else {
      if(end.load()) return;
      sleep_for(seconds(1));
    }
  }
}

void MessageMgr::sendLoop() {
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

void MessageMgr::sendBoundary(NodeID node, Fragment* fragment) {
  Message* message = new Message;
  fragmentTool->boundarySerialize(fragment, message->buffer, message->size);
  message->tag = UPDATE_FRAGMENT;
  message->node = node;
  push(message);
}

void MessageMgr::sendFullFragment(NodeID node, Fragment* fragment) {
  Message* message = new Message;
  message->size = fragmentTool->fullSerialize(fragment, message->buffer);
  message->tag = MOVE_FRAGMENT;
  message->node = node;
  push(message);
}

void MessageMgr::sendReduce(ReduceData* reduceData) {
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

void MessageMgr::stop() {
  end.store(true);
}

void MessageMgr::sendStartMove(NodeID node, const ts::type::ID& id, NodeID to) {
  Message* message = new Message;
  message->node = node;
  message->tag = START_MOVE_FRAGMENT;
  message->size = id.serialize(message->buffer);

  int nodesize = sizeof(NodeID) / sizeof(char);
  char* nbuf = reinterpret_cast<char*>(&to);

  char* buf = new char[nodesize + message->size];
  memcpy(buf, message->buffer, message->size);
  memcpy(buf + message->size, nbuf, nodesize);

  delete[] message->buffer;
  message->buffer = buf;
  message->size += nodesize;

  push(message);
}

void MessageMgr::sendConfirmMove(NodeID node, const ts::type::ID& id) {
  Message* message = new Message;
  message->node = node;
  message->tag = CONFIRM_MOVE_FRAGMENT;
  message->size = id.serialize(message->buffer);

  push(message);
}

void MessageMgr::push(Message* message) {
  queueMutex.lock();
  sendQueue.push(message);
  queueMutex.unlock();
}

Message* MessageMgr::pop() {
  queueMutex.lock();
  Message* message = sendQueue.front();
  sendQueue.pop();
  queueMutex.unlock();
  return message;
}
}}
