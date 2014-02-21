#pragma once
#include <thread>
#include <atomic>

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
  AbstractCellTools* cellTool;

  NodeID id;
  std::thread sender;
  std::thread receiver;
  std::atomic<bool> end;
  std::atomic<std::queue<Message> > sendQueue;
public:
  MessageMgr();
  ~MessageMgr();

  void setCellMgr(CellMgr* mgr) { cellMgr = mgr; }
  void setSystem(System* _sys) { sys = _sys; }
  void setCellTool(AbstractCellTools* tool) { cellTool = tool; }

  void run();
  void join();
  void stop();

  void send(NodeID node, Tag tag, AbstractCell* cell);

  NodeID getNodeID() { return id; }
private:
  void sendLoop();
  void receiveLoop();
};
}
}
