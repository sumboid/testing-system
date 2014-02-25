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
  UPDATE_CELL
};

struct Message {
  void* buffer;
  size_t size;
  Tag tag;
};

class MessageMgr {
private:
  Comm* comm;
  CellMgr* cellMgr;
  System* sys;
  ts::type::AbstractCellTools* cellTool;

  ts::type::NodeID id;
  std::thread sender;
  std::thread receiver;
  std::atomic<bool> end;
  std::queue<Message> sendQueue;
  std::mutex queueMutex;
public:
  MessageMgr();
  ~MessageMgr();

  void setCellMgr(CellMgr* mgr) { cellMgr = mgr; }
  void setSystem(System* _sys) { sys = _sys; }
  void setCellTool(ts::type::AbstractCellTools* tool) { cellTool = tool; }

  void run();
  void join();
  void stop();

  void send(ts::type::NodeID node, Tag tag, ts::type::AbstractCell* cell);

  ts::type::NodeID getNodeID() { return id; }
private:
  void sendLoop();
  void receiveLoop();
};
}
}
