#include "CellMgr.h"

using std::vector;
using std::map;

ts::system::CellMgr() {
  cellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock);
}

ts::system::CellMgr(MessageMgr* msgMgr):
  messageMgr(msgMgr) {
  cellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock);
}

ts::system::~CellMgr() {
  pthread_rwlock_destroy(cellsLock);
  delete cellsLock;
}

void ts::system::addCell(AbstractCell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
}

typedef pair<AbstractCell*, vector<AbstractCell*> > WorkCell;
vector<WorkCell>
ts::system::getCells(int amount) {
  pthread_rwlock_wrlock(cellsLock);
  vector<WorkCell> result;

  vector<AbstractCell*> fcells;
  for(auto cell: cells)
    if(!cell.second) fcells.push_back(cell.first);

  for(auto cell: fcells) {
    vector<AbstractCell::ID> neighboursID = cell.neighbours();
    vector<AbstractCell*> neighbours;

    for(auto i: neighboursID) {
      auto cellit = cells.find(i);
      if (cells.end() == cellit) {
        cellit = externalCells.find(i);
        if (cells.end() == cellit)
          break;
      }
      if(*cellit.first.iteration() < cell.iteration()) break;

      neighbours.push_back(*cellit.first);
    }

    if(neighbours.size() == neighboursID.size())
      result.push_back(pair(cell, neighbours));
  }
  pthread_rwlock_unlock(cellsLock);
  return result;
}

void ts::system::unlock(AbstractCell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
  auto nodes = cell->noticeList();
  for(auto node: nodes) messageMgr->send(node, UPDATE_CELL, cell->serialize());
}

void ts::system::updateExternalCell(AbstractCell* cell) {
  pthread_rwlock_rdlock(cellsLock);
  for(auto fcell: externalCells) {
    if(fcell->id() == cell->id()) {
      auto it = externalCells.find(fcell);
      delete *it;
      *it = cell;
      break;
    }
  }
}