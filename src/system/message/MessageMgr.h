#pragma once
#include <thread>
#include <atomic>
#include <mutex>

#include "comm/Comm.h"
#include "../cell/CellMgr.h"
#include "../System.h"
#include "../../types/AbstractCell.h"

namespace ts {
namespace system {

enum Tag {
  UNDEFINED,
  UPDATE_CELL,
  REDUCE_DATA
};

struct Message {
  char* buffer;
  size_t size;
  Tag tag;
  ts::type::NodeID node;
  Message() {
    buffer = 0;
    size = 0;
    tag = UNDEFINED;
  }
};

class System;
class CellMgr;

class MessageMgr {
private:
  Comm* comm;
  CellMgr* cellMgr;
  System* sys;
  ts::type::AbstractCellTools* cellTool;
  ts::type::ReduceDataTools* reduceTool;

  size_t id;
  size_t _size;
  std::thread sender;
  std::thread receiver;
  std::atomic<bool> end;
  std::queue<Message> sendQueue;
  std::mutex queueMutex;
public:
  MessageMgr();
  ~MessageMgr() {};

  void setCellMgr(CellMgr* mgr) { cellMgr = mgr; }
  void setSystem(System* _sys) { sys = _sys; }
  void setCellTool(ts::type::AbstractCellTools* tool) { cellTool = tool; }
  void setReduceTool(ts::type::ReduceDataTools* tool) { reduceTool = tool; }

  void run();
  void join();
  void stop();

  size_t size() { return _size; }

  void send(ts::type::NodeID node, Tag tag, ts::type::AbstractCell* cell);
  void send(ts::type::ReduceData* reduceData);
  ts::type::NodeID getNodeID() { return id; }
private:
  void sendLoop();
  void receiveLoop();
};
}
}
