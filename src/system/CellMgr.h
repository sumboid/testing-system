#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <pair>

#include "../types/Cell.h"
#include "MessageMgr.h"

namespace ts {
namespace system {
  class CellMgr {
  private:
    MessageMgr* messageMgr;

    std::vector<Cell*> externalCells;
    std::map<Cell*, bool> cells;
    pthread_rwlock_t* cellsLock;

  public:
    CellMgr();
    CellMgr(MessageMgr* msgMgr);
    ~CellMgr();

    void setMessageMgr(MessageMgr*);

    void addCell(Cell* cell);
    std::vector<std::pair<Cell*, std::vector<Cell*> > >
      getCells(int amount);
    void unlock(Cell* cell);

  private:
    std::map<Cell*, bool> filter();
  };
}}
