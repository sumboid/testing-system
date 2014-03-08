#include "ExecMgr.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <cassert>

using std::vector;
using std::thread;
using std::this_thread::sleep_for;
using std::mutex;
using std::unique_lock;
using std::chrono::seconds;
using ts::type::AbstractCell;
using ts::type::ReduceData;

void ts::system::ExecMgr::run() {
  loopThread = thread(&ExecMgr::loop, this);
}

void ts::system::ExecMgr::join() {
  loopThread.join();
}

void ts::system::ExecMgr::stop() {
  end = true;
}

void ts::system::ExecMgr::add(const vector<WorkCell>& cells) {
  queueMutex.lock();
  for(auto cell: cells) {
    cellQueue.push(cell);
  }
  queueMutex.unlock();
  queueListener.notifyAll();
}
void ts::system::ExecMgr::add(const WorkCell& cell) {
  queueMutex.lock();
  cellQueue.push(cell);
  queueMutex.unlock();
  queueListener.notifyAll();
}

/**
 * @brief ts::system::ExecMgr::loop
 * Main loop of Execution Manager. It has two missions:
 * 1. Handling cells: running cell step
 * 2. Ending reduce step
 */

void ts::system::ExecMgr::loop() {
  while(true) {
    if(rstate == PRE_GLOBAL_REDUCING) {

      /// Send reduce data to other nodes
      system->spreadReduceData(localReduceData);
      rstate = GLOBAL_REDUCING;

      /// Waiting for all external reduce data
      ERDListener.wait();

      /// Reduce ExecMgr::localReduceData and ExecMgr::reduceData and store it
      /// to ExecMgr::storedReduceData
      if (storedReduceData != 0) delete storedReduceData;
      storedReduceData = reduceTools->reduce(localReduceData,
                                             externalReduceData);

      /// Clean up ExecMgr::reduceData
      delete externalReduceData;
      externalReduceData = 0;

      /// Clean up ExecMgr::localReduceData
      delete localReduceData;
      localReduceData = 0;

      /// Notify that global reduce step is finished
      globalReduceListener.notifyAll();
    }

    queueMutex.lock();
    bool emptyQueue = cellQueue.empty();
    queueMutex.unlock();

    if(emptyQueue) {
      if(end) return;
      queueListener.wait();
    }

    while(!cellQueue.empty()) {
      queueMutex.lock();
      auto cell = cellQueue.front();
      cellQueue.pop();
      queueMutex.unlock();

      if (!compute(cell)) {
        add(cell);
        break;
      }
    }
  }
}

void ts::system::ExecMgr::reduce(ReduceData* rdata) {
  /// Wating for global reduce step ending
  globalReduceListener.wait();
  if(externalReduceData == 0) {
    externalReduceData = rdata;
  }
  else {
    auto tmp = externalReduceData;
    externalReduceData = reduceTools->reduce(rdata, externalReduceData);
    delete tmp;
  }
}

void ts::system::ExecMgr::endGlobalReduce() {

  /// Stop sending external reduce data, while global reduce finish
  globalReduceListener.condition = false;
  ERDListener.notifyAll();
}

bool ts::system::ExecMgr::compute(WorkCell& cell) {
  if(cell.first->needReduce() && cell.first->wasReduced()) {
    if(rstate == GLOBAL_REDUCING) {
      localReduceData = cell.first->_reduce();
      rstate = LOCAL_REDUCING;
    }
    else if(rstate == LOCAL_REDUCING) {
      auto tmp = localReduceData;
      localReduceData = cell.first->_reduce(localReduceData);
      delete tmp;
    }
    else exit(4); // XXX
  }
  else if(!cell.first->needReduce() && !cell.first->wasReduced()) {
    if(rstate == LOCAL_REDUCING) {
      rstate = PRE_GLOBAL_REDUCING;
      return false;
    }
    cell.first->_reduceStep(storedReduceData);
  }
  else {
    justcompute(cell);
  }
  return true;
}

void ts::system::ExecMgr::justcompute(WorkCell& cell) {
  cell.first->_runStep(cell.second);
}
