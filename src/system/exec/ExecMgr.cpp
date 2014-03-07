#include "ExecMgr.h"

#include <thread>
#include <chrono>
#include <iostream>

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
}
void ts::system::ExecMgr::add(const WorkCell& cell) {
  queueMutex.lock();
  cellQueue.push(cell);
  queueMutex.unlock();
}

void ts::system::ExecMgr::loop() {
  while(true) {
    if(rstate == PRE_GLOBAL_REDUCING) {
      system->spreadReduceData(reduceData);
      rstate = GLOBAL_REDUCING;
      if(!reduceDataFetched) {
        std::unique_lock<std::mutex> lock(fetchReduceDataMutex);
        fetchReduceData.wait(lock, [=](){ bool _ = reduceDataFetched; return _; });
      }
      reduceDataFetched = false;
      if (storedReduceData != 0) delete storedReduceData;
      storedReduceData = reduceTools->reduce(localReduceData, reduceData);
      delete reduceData;
      delete localReduceData;
    }

    queueMutex.lock();
    if(cellQueue.empty()) {
      queueMutex.unlock();

      if(end) return;

      sleep_for(seconds(1));
      continue;
    }

    WorkCell cell = cellQueue.front();
    queueMutex.unlock();
    if (!compute(cell)) {
      add(cell);
    }
  }
}

void ts::system::ExecMgr::reduce(ReduceData* rdata) {
  if(reduceData == 0) {
    reduceData = rdata;
  }
  else {
    auto tmp = reduceData;
    reduceData = reduceTools->reduce(rdata, reduceData);
    delete tmp;
  }
}

void ts::system::ExecMgr::endGlobalReduce() {
  reduceDataFetched = true;
  fetchReduceData.notify_all();
}

bool ts::system::ExecMgr::compute(WorkCell& cell) {
  if(cell.first->needReduce() && cell.first->wasReduced()) {
    std::cout << "REDUCING" << std::endl;
    if(rstate == GLOBAL_REDUCING) {
      std::cout << "change reduce state" << std::endl;
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
    std::cout << "NEXT REDUCING" << std::endl;
    if(rstate == LOCAL_REDUCING) {
      rstate = PRE_GLOBAL_REDUCING;
      return false;
    }
    cell.first->_reduceStep(storedReduceData);
  }
  else {
    std::cout << cell.first->needReduce() << std::endl;
    justcompute(cell);
  }
  return true;
}

void ts::system::ExecMgr::justcompute(WorkCell& cell) {
  cell.first->_runStep(cell.second);
}
