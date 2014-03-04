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

  ts::type::ReduceData* reduceData;
  ts::type::ReduceData* storedReduceData[2];
  std::queue<WorkCell> cellQueue;

  System* system;

  std::thread loopThread;
  std::mutex queueMutex;
  std::atomic<bool> end;

  std::atomic<ReduceState> rstate;
  ts::type::ReduceDataTools* reduceTools;

  std::condition_variable fetchReduceData;
  std::mutex fetchReduceDataMutex;
  std::atomic<bool> reduceDataFetched;
public:
  ExecMgr(ts::type::ReduceDataTools* _reduceTools): reduceData(0),
                                          end(false),
                                          rstate(GLOBAL_REDUCING),
                                          reduceTools(_reduceTools),
                                          reduceDataFetched(false) {
    reduceData = 0;
    storedReduceData[0] = 0;
    storedReduceData[1] = 0;
  }

  ~ExecMgr() {
    delete reduceData;
    delete storedReduceData[0];
    delete storedReduceData[1];
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
