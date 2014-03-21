#include <iostream>
#include <vector>
#include "System.h"

using std::vector;
using ts::type::CellTools;
using ts::type::Cell;
using ts::type::ReduceDataTools;

ts::system::System::System(CellTools* cellTools, ReduceDataTools* reduceTools):
  inputReduceData(0), _end(false), cellListener(true) {
  msgMgr = new MessageMgr;
  cellMgr = new CellMgr;
  execMgr = new ExecMgr(reduceTools);

  msgMgr->setCellMgr(cellMgr);
  msgMgr->setSystem(this);
  msgMgr->setCellTool(cellTools);
  msgMgr->setReduceTool(reduceTools);

  cellMgr->setMessageMgr(msgMgr);
  cellMgr->setSystem(this);
  cellMgr->setCellTools(cellTools);

  execMgr->setSystem(this);

  msgMgr->run();
  execMgr->run();
}

ts::system::System::~System() {
  msgMgr->stop();
  execMgr->stop();
  msgMgr->join();
  execMgr->join();

  delete msgMgr;
  delete execMgr;
  delete cellMgr;
}

void ts::system::System::run() {
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

void ts::system::System::spreadReduceData(ts::type::ReduceData* data) {
  if(msgMgr->size() == 1) {
    execMgr->endGlobalReduce();
    return;
  }
  msgMgr->send(data);
}

void ts::system::System::putReduceData(ts::type::ReduceData* data) {
  execMgr->reduce(data);
  ++inputReduceData;

  if(inputReduceData == msgMgr->size() - 1) {
    inputReduceData = 0;
    execMgr->endGlobalReduce();
  }
  delete data;
}

void ts::system::System::addCell(ts::type::Cell* cell) {
  cell->setNodeID(id());
  cellMgr->addCell(cell);
}

uint64_t ts::system::System::id() {
  return msgMgr->getNodeID();
}

uint64_t ts::system::System::size() {
  return msgMgr->size();
}

void ts::system::System::unlockCell(ts::type::Cell* cell) {
  cellMgr->unlock(cell);
  notify();
}

void ts::system::System::notify() {
  cellListener.notifyAll();
}

vector<Cell*> ts::system::System::getCells() { return cellMgr->getCells(); }
