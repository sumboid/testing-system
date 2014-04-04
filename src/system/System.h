#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"
#include "../types/Cell.h"
#include "../types/ReduceDataTools.h"
#include "../types/CellTools.h"
#include "util/Semaphore.h"
#include "util/Listener.h"

#include "action/Action.h"
#include "action/ActionBuilder.h"

namespace ts {
namespace system {

class CellMgr;
class MessageMgr;
class ExecMgr;
class Action;
class ActionBuilder;

class System {
private:
  MessageMgr* msgMgr;
  CellMgr* cellMgr;
  ExecMgr* execMgr;
  ActionBuilder* actionBuilder;
  size_t inputReduceData;
  std::atomic<bool> _end;

  Semaphore cellListener;

  std::thread actionLoopThread;
  std::mutex queueMutex;
  std::queue<Action*> actionQueue;
  Listener actionQueueListener;

public:
  System(ts::type::CellTools*, ts::type::ReduceDataTools*);
  ~System();

  void addCell(ts::type::Cell* cell);
  uint64_t id();
  uint64_t size();
  void run();
  void end();
  void notify();
  std::vector<ts::type::Cell*> getCells();

  void addAction(Action* action);
  void actionLoop();

  /// ExecMgr part
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);
  void unlockCell(ts::type::Cell* cell);

  //put(Message* message);
};

}
}
