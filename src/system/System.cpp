#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include "System.h"
#include "../util/Uberlogger.h"

UBERINIT;

namespace ts {
namespace system {

using std::vector;
using ts::type::FragmentTools;
using ts::type::Fragment;
using ts::type::ReduceDataTools;

System::System(FragmentTools* fragmentTools, ReduceDataTools* reduceTools):
  inputReduceData(0), _end(false), fragmentListener(true) {
  msgMgr = new MessageMgr;
  fragmentMgr = new FragmentMgr;
  execMgr = new ExecMgr(reduceTools);

  actionBuilder = new ActionBuilder();
  actionBuilder->setFragmentTools(fragmentTools);
  actionBuilder->setExecMgr(execMgr);
  actionBuilder->setReduceDataTools(reduceTools);
  actionBuilder->setFragmentMgr(fragmentMgr);
  actionBuilder->setSystem(this);

  msgMgr->setFragmentMgr(fragmentMgr);
  msgMgr->setSystem(this);
  msgMgr->setFragmentTool(fragmentTools);
  msgMgr->setReduceTool(reduceTools);
  msgMgr->setActionBuilder(actionBuilder);

  fragmentMgr->setMessageMgr(msgMgr);
  fragmentMgr->setSystem(this);
  fragmentMgr->setFragmentTools(fragmentTools);

  execMgr->setSystem(this);

  UTEMPLATE(fragment, "[%nodeid][%name]: %msg");
  UTEMPLATE(state, "[%nodeid][%name]: %msg");
  UTEMPLATE(success, "[%nodeid][%name]: %msg");
  UTEMPLATE(error, "[%nodeid][%name]: %msg");
  UTEMPLATE(arc, "[%nodeid][%name]: %msg");
  UTEMPLATE(move, "[%nodeid][%name]: %msg");
  UREPLACE(fragment, "%nodeid", [&](){return std::to_string(this->id());});
  UREPLACE(state, "%nodeid", [&](){return std::to_string(this->id());});
  UREPLACE(success, "%nodeid", [&](){return std::to_string(this->id());});
  UREPLACE(error, "%nodeid", [&](){return std::to_string(this->id());});
  UREPLACE(arc, "%nodeid", [&](){return std::to_string(this->id());});
  UREPLACE(move, "%nodeid", [&](){return std::to_string(this->id());});
  USTYLE(error, UBOLD | UCOLOR(RED));
  USTYLE(success, UCOLOR(BLUE));
  USTYLE(state, UCOLOR(GREEN));
  UBERTEMPLATE("[%nodeid][%name]: %msg");
  UBERREPLACE("%nodeid", [&](){return std::to_string(this->id());});

  UBERLOG() << "Hello, sweety" << UBEREND();

  actionLoopThread = std::thread(&System::actionLoop, this);
  msgMgr->run();
  execMgr->run();
}

System::~System() {
  msgMgr->stop();
  execMgr->stop();
  msgMgr->join();
  execMgr->join();

  actionLoopThread.join();
  delete msgMgr;
  delete execMgr;
  delete fragmentMgr;
}

void System::run() {
  while(true) {
    auto fragments = fragmentMgr->getFragments(359);
    if(_end) {
      return;
    }
    else if(fragments.empty()) {
      ULOG(default) << "Trying to get fragments" << UEND;
      //fragmentListener.wait();
      sleep(1); // Sort of KOSTYL.
      continue;
    }

    execMgr->add(fragments);
  }
}

void System::spreadReduceData(ts::type::ReduceData* data) {
  if(msgMgr->size() == 1) {
    execMgr->endGlobalReduce();
    return;
  }
  msgMgr->sendReduce(data);
}

void System::putReduceData(ts::type::ReduceData* data) {
  execMgr->reduce(data);
  ++inputReduceData;

  if(inputReduceData == msgMgr->size() - 1) {
    inputReduceData = 0;
    execMgr->endGlobalReduce();
  }
  delete data;
}

void System::addFragment(ts::type::Fragment* fragment) {
  fragment->setNodeID(id());
  fragmentMgr->addFragment(fragment);
}

uint64_t System::id() {
  return msgMgr->getNodeID();
}

uint64_t System::size() {
  return msgMgr->size();
}

void System::unlockFragment(ts::type::Fragment* fragment) {
  fragmentMgr->unlock(fragment);
  notify();
}

void System::notify() {
  fragmentListener.notifyAll();
}

vector<Fragment*> System::getFragments() { return fragmentMgr->getFragments(); }

void System::addAction(Action* action) {
  queueMutex.lock();
  actionQueue.push(action);
  queueMutex.unlock();
  actionQueueListener.notifyAll();
}

void System::actionLoop() {
  while(true) {
    queueMutex.lock();
    size_t queueSize = actionQueue.size();
    queueMutex.unlock();

    if(queueSize == 0) {
      actionQueueListener.wait();

      if(_end) {
        return;
      }

      continue;
    }

    for(size_t i = 0; i < queueSize; ++i) {
      queueMutex.lock();
      Action* action = actionQueue.front();
      actionQueue.pop();
      queueMutex.unlock();

      action->run();
      delete action;
    }
  }
}

void System::end() {
  _end = true;
  fragmentListener.notifyAll();
  actionQueueListener.notifyAll();
}

}}
