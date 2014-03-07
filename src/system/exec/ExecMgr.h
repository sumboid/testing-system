#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <vector>
#include <utility>
#include <queue>

#include "../../types/AbstractCell.h"
#include "../System.h"

namespace ts {
namespace system {

class System;
typedef std::pair<ts::type::AbstractCell*, std::vector<ts::type::AbstractCell*> > WorkCell;
class ExecMgr {
private:
  enum ReduceState { LOCAL_REDUCING, PRE_GLOBAL_REDUCING, GLOBAL_REDUCING };

  struct Listener {
    std::condition_variable listener;
    std::mutex mutex;
    std::atomic<bool> condition;

    Listener(bool begin  = false) {
      condition = begin;
    }

    void wait() {
      std::unique_lock<std::mutex> lock(mutex);
      if(!condition) {
        listener.wait(lock, [=](){ bool _ = condition; return _; });
      }
      condition = false;
      lock.unlock();
    }

    void notifyAll() {
      std::lock_guard<std::mutex> lock(mutex);
      condition = true;
      listener.notify_all();
    }

    void notifyOne() {
      std::lock_guard<std::mutex> lock(mutex);
      condition = true;
      listener.notify_one();
    }
  };

  ts::type::ReduceData* externalReduceData;
  ts::type::ReduceData* localReduceData;
  ts::type::ReduceData* storedReduceData;
  std::queue<WorkCell> cellQueue;

  System* system;

  std::thread loopThread;
  std::mutex queueMutex;
  std::atomic<bool> end;

  std::atomic<ReduceState> rstate;
  ts::type::ReduceDataTools* reduceTools;

  Listener ERDListener; ///< External reduce data listener
  Listener globalReduceListener; ///< Global reduce step listener
  Listener queueListener; ///< Queue listener
public:
  ExecMgr(ts::type::ReduceDataTools* _reduceTools): externalReduceData(0),
                                          end(false),
                                          rstate(GLOBAL_REDUCING),
                                          reduceTools(_reduceTools),
                                          globalReduceListener(true) {
    externalReduceData = 0;
    storedReduceData = 0;
    localReduceData = 0;
  }

  ~ExecMgr() {
    delete externalReduceData;
    delete storedReduceData;
  }

  void setSystem(System* _system) { system = _system; }

  void run();
  void join();
  void stop();

  void add(const std::vector<WorkCell>& cells);
  void add(const WorkCell& cells);

  //Reduce
  void reduce(ts::type::ReduceData* rdata);
  void endGlobalReduce();

private:
  void loop();
  bool compute(WorkCell& cell);
  void justcompute(WorkCell& cell);
};
}
}
