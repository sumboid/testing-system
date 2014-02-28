#include "System.h"

using ts::type::AbstractCellTools;
using ts::type::ReduceDataTools;

ts::system::System::System(AbstractCellTools* cellTools, ReduceDataTools* reduceTools) {
  msgMgr = new MessageMgr;
  cellMgr = new CellMgr;
  execMgr = new ExecMgr;

  msgMgr->setCellMgr(cellMgr);
  msgMgr->setSystem(this);
  msgMgr->setCellTool(cellTools);
  msgMgr->setReduceTool(reduceTools);

  cellMgr->setMessageMgr(msgMgr);

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
    execMgr->add(cells);
  }
}
