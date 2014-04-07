#pragma once
#include <thread>
#include <atomic>
#include <mutex>

#include "comm/Comm.h"
#include "../cell/CellMgr.h"
#include "../System.h"
#include "../../types/Cell.h"
#include "../../types/CellTools.h"
#include "../../types/ReduceDataTools.h"
#include "../action/ActionBuilder.h"

namespace ts {
namespace system {

class ActionBuilder;

typedef int NodeID;

enum Tag {
  UNDEFINED,
  UPDATE_CELL,       ///< Updating cell from external Node.
  REDUCE_DATA,       ///< Partial reduce data

  START_MOVE_CELL,   ///< Beginning of moving cell process
  CONFIRM_MOVE_CELL, ///< Confirming of moving cell
  MOVE_CELL          ///< Moving cell
};

struct Message {
  char* buffer;
  size_t size;
  Tag tag;
  NodeID node;
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
  ActionBuilder* actionBuilder;

  ts::type::CellTools* cellTool;
  ts::type::ReduceDataTools* reduceTool;

  size_t id;
  size_t _size;
  std::thread sender;
  std::thread receiver;
  std::atomic<bool> end;
  std::queue<Message*> sendQueue;
  std::mutex queueMutex;
public:
  MessageMgr();
  ~MessageMgr();

  void setCellMgr(CellMgr* mgr) { cellMgr = mgr; }
  void setSystem(System* _sys) { sys = _sys; }
  void setCellTool(ts::type::CellTools* tool) { cellTool = tool; }
  void setReduceTool(ts::type::ReduceDataTools* tool) { reduceTool = tool; }
  void setActionBuilder(ActionBuilder* ab) { actionBuilder = ab; }

  void run();
  void join();
  void stop();

  size_t size() { return _size; }

  void send(NodeID node, Tag tag, ts::type::Cell* cell);
  void send(ts::type::ReduceData* reduceData);
  NodeID getNodeID() { return id; }
private:
  void sendLoop();
  void receiveLoop();
};
}
}
