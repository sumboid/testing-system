#include "System.h"

ts::system::System::System() {
  messageMgr = new MessageMgr;
  cellMgr = new CellMgr;
  execMgr = new ExecMgr;

  messageMgr.setCellMgr(cellMgr);
  messageMgr.setSystem(this);
  setCellTool(); //XXX

  cellMgr.setMessageMgr(messageMgr);

  execMgr.setSystem(this);

  messageMgr.run();
  execMgr.run();
}

ts::system::System::~System() {
  messageMgr.stop();
  execMgr.stop();
  messageMgr.join();
  execMgr.join();

  delete messageMgr;
  delete execMgr;
  delete cellMgr;
}

void ts::system::System::run() {
  while(true) {
    auto cells = cellMgr.getCells(359);
    for(auto cell: cells)
      execMgr.add(cell)
  }
}
