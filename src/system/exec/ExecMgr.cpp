#include "ExecMgr.h"

#include <thread>
#include <chrono>

using std::vector;
using std::thread;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using ts::type::AbstractCell;

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
    queueMutex.lock();
  }
  queueMutex.unlock();
}

void ts::system::ExecMgr::loop() {
  while(true) {
    queueMutex.lock();
    if(cellQueue.empty()) {
      queueMutex.unlock();
      if(end) return;

      sleep_for(seconds(1));
      continue;
    }

    WorkCell cell = cellQueue.front();
    queueMutex.unlock();
    compute(cell);
  }
}

void ts::system::ExecMgr::compute(WorkCell& cell) {

}
