#include "ExecMgr.h"

#include <chrono>

using std::vector;
using std::thread;
using std::this_thread;
using std::chrono;


void ts::system::run() {
  loopThread = thread(&ExecMgr::loop, this); 
}

void ts::system::join() {
  loopThread.join();
}

void ts::system::add(const vector<WorkCell>& cells) {
  queueMutex.lock();
  for(auto cell: cells) {
    cellQueue.push_back(cell);
  }
  queueMutex.unlock();
}

void ts::system::loop() {
  while(true) {
    queueMutex.lock();
    if(cellQueue.empty()) {
      queueMutex.unlock();
      if(end) return;
      this_thread::sleep_for(chrono::seconds(1));
      continue;
    }

    WorkCell cell = cellQueue.pop_front();
    queueMutex.unlock();
    compute(cell);
  }
}

void ts::system::compute(WorkCell& cell) {
  
}