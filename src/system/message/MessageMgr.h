#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <set>

#include "NodeID.h"
#include "comm/Comm.h"
#include "../fragment/FragmentMgr.h"
#include "../System.h"
#include "../../types/Fragment.h"
#include "../../types/FragmentTools.h"
#include "../../types/ReduceDataTools.h"
#include "../action/ActionBuilder.h"

namespace ts {
namespace system {

class ActionBuilder;

enum Tag {
  UNDEFINED,
  UPDATE_FRAGMENT,       ///< Updating fragment from external Node.
  REDUCE_DATA,           ///< Partial reduce data

  START_MOVE_FRAGMENT,   ///< Beginning of moving fragment process
  NOTICE_MOVE_FRAGMENT,
  CONFIRM_MOVE_FRAGMENT, ///< Confirming of moving fragment
  GLOBAL_CONFIRM_MOVE_FRAGMENT, ///< Confirming of moving fragment
  MOVE_FRAGMENT,          ///< Moving fragment
  LOAD
};

struct Message {
  char* buffer;
  size_t size;
  Tag tag;
  ts::NodeID node;
  Message() {
    buffer = 0;
    size = 0;
    tag = UNDEFINED;
    node = 0;
  }
};

class System;
class FragmentMgr;

class MessageMgr {
private:
  Comm* comm;
  FragmentMgr* fragmentMgr;
  System* sys;
  ActionBuilder* actionBuilder;

  ts::type::FragmentTools* fragmentTool;
  ts::type::ReduceDataTools* reduceTool;

  size_t id;
  size_t _size;
  std::thread sender;
  std::thread receiver;
  std::atomic<bool> end;
  std::queue<Message*> sendQueue;
  std::mutex queueMutex;

  MessageMgr(const MessageMgr&) {}
public:
  MessageMgr();
  ~MessageMgr();

  void setFragmentMgr(FragmentMgr* mgr) { fragmentMgr = mgr; }
  void setSystem(System* _sys) { sys = _sys; }
  void setFragmentTool(ts::type::FragmentTools* tool) { fragmentTool = tool; }
  void setReduceTool(ts::type::ReduceDataTools* tool) { reduceTool = tool; }
  void setActionBuilder(ActionBuilder* ab) { actionBuilder = ab; }

  void run();
  void join();
  void stop();

  size_t size() { return _size; }

  void sendBoundary(ts::NodeID node, ts::type::Fragment* fragment);
  void sendFullFragment(ts::NodeID node, ts::type::Fragment* fragment);
  void sendReduce(ts::type::ReduceData* reduceData);
  void sendStartMove(ts::NodeID node, const ts::type::ID& id, ts::NodeID to);
  void sendNoticeMove(ts::NodeID node, const ts::type::ID& id, ts::NodeID to);
  void sendConfirmMove(ts::NodeID node, const ts::type::ID& id);
  void sendGlobalConfirmMove(ts::NodeID node, const ts::type::ID& id);
  void sendLoad(ts::NodeID node, int amount);

  ts::NodeID getNodeID() { return id; }
  std::set<ts::NodeID> getNeighbours();
private:
  void sendLoop();
  void receiveLoop();

  void push(Message* message);
  Message* pop();
};
}
}
