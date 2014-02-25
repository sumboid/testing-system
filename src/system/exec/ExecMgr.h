#pragma once

#include <thread>
#include <mutex>
#include <atomic>

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
    ts::type::ReduceData* reduceData;
    std::queue<WorkCell> cellQueue;

    System* system;

    std::thread loopThread;
    std::mutex queueMutex;
    std::atomic<bool> end;

  public:
    ExecMgr(): reduceData(0), end(false) {}
    ~ExecMgr() {}

    void setSystem(System* _system) { system = _system; }

    void run();
    void join();
    void stop();

    void add(const std::vector<WorkCell>& cells);

  private:
    void loop();
    void compute(WorkCell& cell);
  };
}
}
