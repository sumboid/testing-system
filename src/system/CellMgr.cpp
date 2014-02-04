#include "CellMgr.h"

#include <algorithm>

using std::for_each;
using std::pair;
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

void ts::system::addCell(Cell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
}

typedef pair<Cell*, vector<Cell*> > WorkCell;
vector<WorkCell>
ts::system::getCells(int amount) {
  pthread_rwlock_wrlock(cellsLock);
  vector<WorkCell> result;

  map<Cell*, bool> fcells =  filter();
  if (fcell.empty()) break;


  for(auto cell: fcells) {
    vector<Cell::ID> neighboursID = cell.neighbours();
    vector<Cell*> neighbours;

    for(auto i: neighboursID) {
      auto cellit = cells.find(i);
      if (cells.end() == cellit)
        // find in external cells
        messageMgr.send(CELLMGR_TAG, NEED_CELL, i);
        break; //send query to neighbours and drop current cell
      if(*cellit.first.iteration() < cell.iteration()) break;

      neighbours.push_back(*cellit.first);
    }

    if(neighbours.size() == neighboursID.size())
      result.push_back(pair(cell, neighbours));
  }

  pthread_rwlock_unlock(cellsLock);
}
