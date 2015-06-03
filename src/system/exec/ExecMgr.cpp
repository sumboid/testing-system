#include "ExecMgr.h"

#include <thread>
#include <chrono>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include "../../util/Uberlogger.h"
#include "../action/Action.h"
#include "../action/actions/Unlock.h"

using std::vector;
using std::thread;
using std::this_thread::sleep_for;
using std::mutex;
using std::unique_lock;
using std::chrono::seconds;
using ts::type::Fragment;
using ts::type::ReduceData;

void ts::system::ExecMgr::run() {
  loopThread = thread(&ExecMgr::loop, this);
}

void ts::system::ExecMgr::join() {
  loopThread.join();
}

void ts::system::ExecMgr::stop() {
  end.store(true);
  queueListener.notifyAll();
}

void ts::system::ExecMgr::add(const vector<WorkFragment>& fragments) {
  queueMutex.lock();
  for(auto fragment: fragments) {
    fragmentQueue.push(fragment);
  }
  queueMutex.unlock();
  queueListener.notifyAll();
}
void ts::system::ExecMgr::add(const WorkFragment& fragment) {
  queueMutex.lock();
  fragmentQueue.push(fragment);
  queueMutex.unlock();
  queueListener.notifyAll();
}

/**
 * @brief ts::system::ExecMgr::loop
 * Main loop of Execution Manager. It has two missions:
 * 1. Handling fragments: running fragment step
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
      if (externalReduceData != 0) {
        storedReduceData = reduceTools->reduce(localReduceData,
                                             externalReduceData);
        /// Clean up ExecMgr::reduceData
        delete externalReduceData;
        externalReduceData = 0;

        /// Clean up ExecMgr::localReduceData
        delete localReduceData;
        localReduceData = 0;
      }
      else {
        storedReduceData = localReduceData;
      }

      /// Notify that global reduce step is finished
      globalReduceListener.notifyAll();
    }

    queueMutex.lock();
    bool emptyQueue = fragmentQueue.empty();
    queueMutex.unlock();

    if(emptyQueue) {
      system->notify();
      queueListener.wait();
      if(end.load()) {
        return;
      }
    }

    while(!fragmentQueue.empty()) {
      queueMutex.lock();
      auto fragment = fragmentQueue.front();
      //ULOG(exec) << fragment.first->id().tostr() << UEND;
      fragmentQueue.pop();
      queueMutex.unlock();

      if (!compute(fragment)) {
        add(fragment);
        break;
      }
      action::Unlock* action = new action::Unlock();
      action->setFragmentMgr(fragmentMgr);
      action->set(fragment.first);
      system->addAction(action);
    }
  }
}

void ts::system::ExecMgr::reduce(ReduceData* rdata) {
  /// Wating for global reduce step ending
  globalReduceListener.wait();
  globalReduceListener.invert();
  if(externalReduceData == 0) {
    externalReduceData = rdata->copy();
  }
  else {
    auto tmp = externalReduceData;
    externalReduceData = reduceTools->reduce(rdata, externalReduceData);
    delete tmp;
  }
}

void ts::system::ExecMgr::endGlobalReduce() {

  /// Stop sending external reduce data, while global reduce finish
  globalReduceListener.invert();
  ERDListener.notifyAll();
}

bool ts::system::ExecMgr::compute(WorkFragment& fragment) {
  if(fragment.first->needReduce() && fragment.first->wasReduced()) {
    if(rstate == GLOBAL_REDUCING) {
      localReduceData = fragment.first->_reduce();
      rstate = LOCAL_REDUCING;
    }
    else if(rstate == LOCAL_REDUCING) {
      auto tmp = localReduceData;
      localReduceData = fragment.first->_reduce(localReduceData);
      delete tmp;
    }
    else exit(4); // XXX
  }
  else if(!fragment.first->needReduce() && !fragment.first->wasReduced()) {
    if(rstate == LOCAL_REDUCING) {
      rstate = PRE_GLOBAL_REDUCING;
      return false;
    }
    fragment.first->_reduceStep(storedReduceData);
  }
  else {
    justcompute(fragment);
  }
  return true;
}

void ts::system::ExecMgr::justcompute(WorkFragment& fragment) {
  fragment.first->_runStep(fragment.second);
}
