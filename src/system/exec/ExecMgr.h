#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include <vector>
#include <utility>
#include <queue>

#include "../../types/Fragment.h"
#include "../../types/ReduceData.h"
#include "../../types/ReduceDataTools.h"
#include "../System.h"
#include "../util/Listener.h"

namespace ts {
namespace system {

class System;
typedef std::pair<ts::type::Fragment*, std::vector<ts::type::Fragment*> > WorkFragment;
class ExecMgr {
private:
  enum ReduceState { LOCAL_REDUCING, PRE_GLOBAL_REDUCING, GLOBAL_REDUCING };
  ts::type::ReduceData* externalReduceData;
  ts::type::ReduceData* localReduceData;
  ts::type::ReduceData* storedReduceData;
  std::queue<WorkFragment> fragmentQueue;

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

  void add(const std::vector<WorkFragment>& fragments);
  void add(const WorkFragment& fragments);

  //Reduce
  void reduce(ts::type::ReduceData* rdata);
  void endGlobalReduce();

private:
  void loop();
  bool compute(WorkFragment& fragment);
  void justcompute(WorkFragment& fragment);
};
}
}
