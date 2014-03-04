#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <utility>

#include "../../types/AbstractCell.h"
#include "../message/MessageMgr.h"

namespace ts {
namespace system {
  class System;
  class MessageMgr;

  class CellMgr {
  private:
    MessageMgr* messageMgr;

    std::vector<ts::type::AbstractCell*> externalCells;
    std::map<ts::type::AbstractCell*, bool> cells;
    pthread_rwlock_t* cellsLock;

  public:
    CellMgr();
    CellMgr(MessageMgr* msgMgr);
    ~CellMgr();

    void setMessageMgr(MessageMgr* mgr) { messageMgr = mgr; }

    void addCell(ts::type::AbstractCell* cell);
    std::vector<std::pair<ts::type::AbstractCell*, std::vector<ts::type::AbstractCell*> > >
      getCells(int amount);
    void unlock(ts::type::AbstractCell* cell);
    void updateExternalCell(ts::type::AbstractCell* cell);
  };
}}
