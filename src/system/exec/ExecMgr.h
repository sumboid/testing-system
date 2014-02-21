#pragma once

#include <thread>
#include <mutex>

#include <vector>
#include <pair>
#include <queue>

#include "../types/AbstractCell.h"

namespace ts {
namespace system {
  typedef std::pair<AbstractCell*, std::vector<AbstractCell*> > WorkCell;
  class ExecMgr {
  private:
    ReduceData* reduceData;
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
