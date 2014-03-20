#include "CellMgr.h"

#include <algorithm>

using std::pair;
using std::vector;
using std::map;
using ts::type::Cell;
using ts::type::ID;

ts::system::CellMgr::CellMgr() {
  cellsLock = new pthread_rwlock_t;
  externalCellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock, NULL);
  pthread_rwlock_init(externalCellsLock, NULL);
}

ts::system::CellMgr::CellMgr(MessageMgr* msgMgr):
  messageMgr(msgMgr) {
  cellsLock = new pthread_rwlock_t;
  pthread_rwlock_init(cellsLock, NULL);
}

ts::system::CellMgr::~CellMgr() {
  pthread_rwlock_destroy(cellsLock);
  pthread_rwlock_destroy(externalCellsLock);
  delete cellsLock;
  delete externalCellsLock;
}

void ts::system::CellMgr::addCell(Cell* cell) {
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
}

typedef pair<Cell*, vector<Cell*> > WorkCell;
vector<WorkCell> ts::system::CellMgr::getCells(int amount) {
  vector<WorkCell> result;
  vector<WorkCell> reduceResult;
  size_t reduceCount = 0;
  size_t endCount = 0;

  /// Find all non-blocked cells without end state.
  vector<Cell*> fcells;

  pthread_rwlock_rdlock(cellsLock);
  for(auto cell: cells)
    if(!cell.second) {
      if(!cell.first->isEnd()) {
        fcells.push_back(cell.first);
      }
      else {
        ++endCount;
      }
    }

  /// If all of cells are free with end state shut down the System
  if(endCount == cells.size()) {
    system->end();
    pthread_rwlock_unlock(cellsLock);
    return vector<WorkCell>();
  }

  pthread_rwlock_unlock(cellsLock);

  for(auto cell: fcells) {
    /// Check cell waiting for final reduce step
    if(!cell->needReduce() && !cell->wasReduced()) {
      ++reduceCount;
      reduceResult.push_back(WorkCell(cell, vector<Cell*>()));
      break;
    }

    if(!cell->needNeighbours()) {
      result.push_back(WorkCell(cell, vector<Cell*>()));
      break;
    }

    vector<ID> neighboursID = cell->neighbours();
    vector<Cell*> neighbours;

    /// Find all neighbours of current cell
    for(auto i: neighboursID) {
      Cell* findedCell;

      /// Find neighbour cell in local cells
      auto cellit = find_if(cells.begin(), cells.end(),
                           [&i](pair<Cell*, bool> cell){
        return *cell.first == i;
      });

      if (cells.end() == cellit) {
        /// If neighbour cell doesn't exist in local cells
        /// try to find it in external cells

        pthread_rwlock_rdlock(externalCellsLock);
        auto cellit = find_if(externalCells.begin(),
                              externalCells.end(), 
                              [&i](Cell* cell) {
          return *cell == i;
        });

        if (externalCells.end() == cellit) {
          pthread_rwlock_unlock(externalCellsLock);
          break;
        }
        else
          findedCell = *cellit;
        pthread_rwlock_unlock(externalCellsLock);
      } else {
        findedCell = cellit->first;
      }

      if(!findedCell->hasState(cell->neighboursState())) { // Need to change
        /// It means that finded copy of external cell has
        /// too old state
        //uint64_t one, two;
        //std::tie(one, two) = cell->neighboursState();
        //std::cout << "NEED " << one << ":" << two << std::endl;
        //std::cout << "HAS" << std::endl; 
        //findedCell->printStates();
        break;
      }
      neighbours.push_back(findedCell->getState(cell->neighboursState(), cell->id()));
    }

    if(neighbours.size() == neighboursID.size()) {
        result.push_back(pair<Cell*, vector<Cell*>>(cell, neighbours));
    }
  }

  pthread_rwlock_wrlock(cellsLock);
  if(reduceCount == cells.size()) {
    for(auto i: reduceResult)
      cells[i.first] = true;
    pthread_rwlock_unlock(cellsLock);

    return reduceResult;
  }
  else {
    for(auto i: result)
      cells[i.first] = true;
    pthread_rwlock_unlock(cellsLock);
    return result;
  }
}

void ts::system::CellMgr::unlock(Cell* cell) {
  if(cell->needUpdate()) {
    auto nodes = cell->noticeList();
    for(auto node: nodes) messageMgr->send(node, UPDATE_CELL, cell->getLastState());
  }
  pthread_rwlock_wrlock(cellsLock);
  cells[cell] = false;
  pthread_rwlock_unlock(cellsLock);
}

void ts::system::CellMgr::updateExternalCell(Cell* cell) {
  //std::cout << "It's new external cell here: " << cell->id().tostr() << 
  //  " with stamp: " << cell->iteration() << ":" << cell->progress() << std::endl;
  pthread_rwlock_rdlock(externalCellsLock);

  for(auto fcell: externalCells) {
    if(fcell->id() == cell->id()) {
      auto it = find(externalCells.begin(), externalCells.end(), fcell);

      pthread_rwlock_unlock(externalCellsLock);
      pthread_rwlock_wrlock(externalCellsLock);

      (*it)->saveState(cell);

      pthread_rwlock_unlock(externalCellsLock);
      system->notify();
      return;
    }
  }

  pthread_rwlock_unlock(externalCellsLock);
  pthread_rwlock_wrlock(externalCellsLock);

  ID cellID = cell->id();
  Cell* newCell = cellTools->createGap(cellID);
  newCell->saveState(cell);
  externalCells.push_back(newCell);
  pthread_rwlock_unlock(externalCellsLock);
  system->notify();
}

vector<Cell*> ts::system::CellMgr::getCells() {
  vector<Cell*> result;

  for(auto cell : cells) {
    result.push_back(cell.first);
  }

  return result;
}
