#include "CellMgr.h"

using std::pair;
using std::vector;
using std::map;
using ts::type::AbstractCell;
using ts::type::ID;

ts::system::CellMgr::CellMgr() {
  cellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock, NULL);
}

ts::system::CellMgr::CellMgr(MessageMgr* msgMgr):
  messageMgr(msgMgr) {
  cellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock, NULL);
}

ts::system::CellMgr::~CellMgr() {
  pthread_rwlock_destroy(cellsLock);
  delete cellsLock;
}

void ts::system::CellMgr::addCell(AbstractCell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
}

typedef pair<AbstractCell*, vector<AbstractCell*> > WorkCell;
vector<WorkCell> ts::system::CellMgr::getCells(int amount) {
  pthread_rwlock_wrlock(cellsLock);
  vector<WorkCell> result;
  vector<pair<AbstractCell*, bool>*> srcResult;
  vector<WorkCell> reduceResult;
  vector<pair<AbstractCell*, bool>*> srcReduceResult;
  size_t reduceCount = 0;
  size_t endCount = 0;

  /// Find all non-blocked cells without end state.
  vector<pair<AbstractCell*, bool>> fcells;
  for(auto cell: cells)
    if(!cell.second) {
      if(!cell.first->isEnd()) {
        fcells.push_back(cell);
      }
      else {
        ++endCount;
      }
    }

  /// If all of cells are free with end state shut down the System
  if(endCount == cells.size()) {
    system->end();
    return vector<WorkCell>();
  }

  for(auto cell: fcells) {
    bool notReduced = false;
    auto rcell = cell.first;
    /// Check cell waiting for final reduce step
    if(!rcell->needReduce() && !rcell->wasReduced()) {
      ++reduceCount;
      notReduced = true;
    }

    vector<ID> neighboursID = rcell->neighbours();
    vector<AbstractCell*> neighbours;

    /// Find all neighbours of current cell
    for(auto i: neighboursID) {
      AbstractCell* findedCell;
      auto cellit = find_if(cells.begin(), cells.end(), [&i](pair<AbstractCell*, bool> cell){ return *cell.first == i; });
      if (cells.end() == cellit) {
        auto cellit = find_if(externalCells.begin(), externalCells.end(), [&i](AbstractCell* cell){ return *cell == i; });
        if (externalCells.end() == cellit)
          break;
        else {
          findedCell = *cellit;
        }
      } else {
        findedCell = cellit->first;
      }
      if(findedCell->iteration() < rcell->iteration()) break;

      neighbours.push_back(findedCell);
    }

    if(neighbours.size() == neighboursID.size()) {
      if(notReduced) {
        srcReduceResult.push_back(&cell);
        reduceResult.push_back(pair<AbstractCell*, vector<AbstractCell*>>(rcell, neighbours));
      }
      else {
        srcResult.push_back(&cell);
        result.push_back(pair<AbstractCell*, vector<AbstractCell*>>(rcell, neighbours));
      }
    }
  }

  if(reduceCount == cells.size()) {
    for(auto src: srcReduceResult)
      src->second = true;

    pthread_rwlock_unlock(cellsLock);

    return reduceResult;
  }
  else {
    for(auto src: srcResult)
      src->second = true;

    pthread_rwlock_unlock(cellsLock);

    return result;
  }
}

void ts::system::CellMgr::unlock(AbstractCell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
  if(cell->needUpdate()) {
    auto nodes = cell->noticeList();
    for(auto node: nodes) messageMgr->send(node, UPDATE_CELL, cell);
  }
}

void ts::system::CellMgr::updateExternalCell(AbstractCell* cell) {
  pthread_rwlock_rdlock(cellsLock);

  bool newCell = true;
  for(auto fcell: externalCells) {
    if(fcell->id() == cell->id()) {
      auto it = find(externalCells.begin(), externalCells.end(), fcell);

      pthread_rwlock_unlock(cellsLock);
      pthread_rwlock_wrlock(cellsLock);

      (*it)->update(cell);

      pthread_rwlock_unlock(cellsLock);
      return;
    }
  }

  pthread_rwlock_unlock(cellsLock);
  pthread_rwlock_wrlock(cellsLock);
  if(newCell) externalCells.push_back(cell);
  pthread_rwlock_unlock(cellsLock);
}
