#include "MessageMgr.h"
#include <iostream>
#include <chrono>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../../util/Uberlogger.h"
#include "../../util/Arc.h"

using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using ts::type::Fragment;
using ts::type::FragmentTools;
using ts::type::ReduceDataTools;
using ts::type::ReduceData;
using ts::Arc;
using ts::NodeID;

namespace ts {
namespace system {

MessageMgr::MessageMgr():
  fragmentMgr(0),
  sys(0),
  actionBuilder(0),
  fragmentTool(0),
  reduceTool(0),
  end(false) {
  comm = new Comm(0, 0);
  id = comm->getRank();
  _size = comm->getSize();
}

MessageMgr::~MessageMgr() {
  delete comm;
}

void MessageMgr::run() {
  receiver = thread(&MessageMgr::receiveLoop, this);
  comm->barrier();
  sender = thread(&MessageMgr::sendLoop, this);
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
      delete[] message.buffer;
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
    queueMutex.lock();
    if(sendQueue.empty()) {
      queueMutex.unlock();
      if(end.load()) return;
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
  Arc* arc = fragmentTool->boundarySerialize(fragment);
  message->size = arc->size();
  message->buffer = arc->get();
  message->tag = UPDATE_FRAGMENT;
  message->node = node;
  delete arc;
  push(message);
}

void MessageMgr::sendFullFragment(NodeID node, Fragment* fragment) {
  Message* message = new Message;
  //UBERLOG() << "Sending full fragment: " << fragment->id().tostr() << UBEREND();
  Arc* arc = fragmentTool->fullSerialize(fragment);
  message->size = arc->size();
  message->buffer = arc->get();
  message->tag = MOVE_FRAGMENT;
  message->node = node;
  delete arc;
  push(message);
}

void MessageMgr::sendReduce(ReduceData* reduceData) {
  queueMutex.lock();
  for(size_t i = 0; i < _size; ++i) {
    if(i != id) {
      Message* message = new Message;
      Arc* arc = reduceTool->serialize(reduceData);
      message->size = arc->size();
      message->buffer = arc->get();
      message->tag = REDUCE_DATA;
      message->node = i;
      delete arc;
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
  Arc* arc = new Arc;
  Arc& a = *arc;
  id.serialize(arc);
  a << to;
  message->size = arc->size();
  message->buffer = arc->get();

  delete arc;

  push(message);
}

void MessageMgr::sendNoticeMove(NodeID node, const ts::type::ID& id, NodeID to) {
  Message* message = new Message;
  message->node = node;
  message->tag = NOTICE_MOVE_FRAGMENT;
  Arc* arc = new Arc;
  Arc& a = *arc;
  id.serialize(arc);
  a << to;
  message->size = arc->size();
  message->buffer = arc->get();

  delete arc;

  push(message);
}

void MessageMgr::sendConfirmMove(NodeID node, const ts::type::ID& id) {
  Message* message = new Message;
  message->node = node;
  message->tag = CONFIRM_MOVE_FRAGMENT;
  Arc* arc = new Arc;
  id.serialize(arc);
  message->size = arc->size();
  message->buffer = arc->get();
  delete arc;

  push(message);
}

void MessageMgr::sendGlobalConfirmMove(NodeID node, const ts::type::ID& id) {
  Message* message = new Message;
  message->node = node;
  message->tag = GLOBAL_CONFIRM_MOVE_FRAGMENT;
  Arc* arc = new Arc;
  id.serialize(arc);
  message->size = arc->size();
  message->buffer = arc->get();
  delete arc;

  push(message);
}

void MessageMgr::sendLoad(ts::NodeID node, uint64_t amount) {
  Message* message = new Message;
  message->node = node;
  message->tag = LOAD;
  Arc* arc = new Arc;
  Arc& a = *arc;
  a << amount;
  message->size = arc->size();
  message->buffer = arc->get();
  delete arc;

  push(message);
}

void MessageMgr::sendHalt() {
  for(int i = 0; i < comm->getSize(); ++i)
    if(i != comm->getRank()) {
      Message* message = new Message;
      message->node = i;
      message->tag = HALT;
      Arc* arc = new Arc;
      Arc& a = *arc;
      a << 0;
      message->size = arc->size();
      message->buffer = arc->get();
      delete arc;

      push(message);
    }
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

std::set<ts::NodeID> MessageMgr::getNeighbours() {
  std::set<ts::NodeID> neighbours;

  if(id == 0) {
    neighbours.emplace(id + 1);
    if(_size != 2) neighbours.emplace(_size - 1);
  } else if (id == _size - 1) {
    neighbours.emplace(0);
    if(_size != 2) neighbours.emplace(id - 1);
  } else {
    neighbours.emplace(id - 1);
    neighbours.emplace(id + 1);
  }

  auto msg = ULOG(error) << "Neighbours: ";
  for(auto& n : neighbours) {
      msg << n << " ";
  }

  msg << UEND;


  return neighbours;
}
}}
