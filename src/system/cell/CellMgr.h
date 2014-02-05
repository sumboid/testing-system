#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <pair>

#include "../types/AbstractCell.h"
#include "MessageMgr.h"

namespace ts {
namespace system {
  class CellMgr {
  private:
    MessageMgr* messageMgr;

    std::vector<AbstractCell*> externalCells;
    std::map<AbstractCell*, bool> cells;
    pthread_rwlock_t* cellsLock;

  public:
    CellMgr();
    CellMgr(MessageMgr* msgMgr);
    ~CellMgr();

    void setMessageMgr(MessageMgr*);

    void addCell(AbstractCell* cell);
    std::vector<std::pair<AbstractCell*, std::vector<AbstractCell*> > >
      getCells(int amount);
    void unlock(AbstractCell* cell);
    void updateExternalCell(AbstractCell* cell);
  };
}}
