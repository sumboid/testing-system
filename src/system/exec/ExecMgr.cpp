#include "ExecMgr.h"

#include <chrono>

using std::vector;
using std::thread;
using std::this_thread;
using std::chrono;


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
    cellQueue.push_back(cell);
  }
  queueMutex.unlock();
}

void ts::system::ExecMgr::loop() {
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

void ts::system::ExecMgr::compute(WorkCell& cell) {

}
