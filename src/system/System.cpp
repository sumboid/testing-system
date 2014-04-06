#include <iostream>
#include <vector>
#include "System.h"

namespace ts {
namespace system {

using std::vector;
using ts::type::CellTools;
using ts::type::Cell;
using ts::type::ReduceDataTools;

System::System(CellTools* cellTools, ReduceDataTools* reduceTools):
  inputReduceData(0), _end(false), cellListener(true) {
  msgMgr = new MessageMgr;
  cellMgr = new CellMgr;
  execMgr = new ExecMgr(reduceTools);

  actionBuilder = new ActionBuilder();
  actionBuilder->setCellTools(cellTools);
  actionBuilder->setExecMgr(execMgr);
  actionBuilder->setReduceDataTools(reduceTools);
  actionBuilder->setCellMgr(cellMgr);
  actionBuilder->setSystem(this);

  msgMgr->setCellMgr(cellMgr);
  msgMgr->setSystem(this);
  msgMgr->setCellTool(cellTools);
  msgMgr->setReduceTool(reduceTools);
  msgMgr->setActionBuilder(actionBuilder);

  cellMgr->setMessageMgr(msgMgr);
  cellMgr->setSystem(this);
  cellMgr->setCellTools(cellTools);

  execMgr->setSystem(this);

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
  delete cellMgr;
}

void System::run() {
  while(true) {
    auto cells = cellMgr->getCells(359);
    if(_end) {
      return;
    }
    else if(cells.empty()) {
      cellListener.wait();
    }

    execMgr->add(cells);
  }
}

void System::spreadReduceData(ts::type::ReduceData* data) {
  if(msgMgr->size() == 1) {
    execMgr->endGlobalReduce();
    return;
  }
  msgMgr->send(data);
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

void System::addCell(ts::type::Cell* cell) {
  cell->setNodeID(id());
  cellMgr->addCell(cell);
}

uint64_t System::id() {
  return msgMgr->getNodeID();
}

uint64_t System::size() {
  return msgMgr->size();
}

void System::unlockCell(ts::type::Cell* cell) {
  cellMgr->unlock(cell);
  notify();
}

void System::notify() {
  cellListener.notifyAll();
}

vector<Cell*> System::getCells() { return cellMgr->getCells(); }

void System::addAction(Action* action) {
  queueMutex.lock();
  actionQueue.push(action);
  queueMutex.unlock();
  actionQueueListener.notifyAll();
}

void System::actionLoop() {
  while(true) {
    queueMutex.lock();
    if(actionQueue.empty()) {
      queueMutex.unlock();
      std::cout << "I'M FUCKING WAITING!" << std::endl;
      actionQueueListener.wait();
      if(_end) {
        return;
      }
    }

    queueMutex.lock();
    size_t queueSize = actionQueue.size();
    queueMutex.unlock();

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
  cellListener.notifyAll();
  actionQueueListener.notifyAll();
}

}}
