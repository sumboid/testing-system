#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <utility>

#include "../../types/Cell.h"
#include "../message/MessageMgr.h"
#include "../System.h"

namespace ts {
namespace system {
  class System;
  class MessageMgr;

  class CellMgr {
  private:
    MessageMgr* messageMgr;
    System* system;

    std::vector<ts::type::Cell*> externalCells;
    std::map<ts::type::Cell*, bool> cells;
    pthread_rwlock_t* cellsLock;

  public:
    CellMgr();
    CellMgr(MessageMgr* msgMgr);
    ~CellMgr();

    void setMessageMgr(MessageMgr* mgr) { messageMgr = mgr; }
    void setSystem(System* _system) { system = _system; }

    void addCell(ts::type::Cell* cell);
    std::vector<std::pair<ts::type::Cell*, std::vector<ts::type::Cell*> > >
      getCells(int amount);
    void unlock(ts::type::Cell* cell);
    void updateExternalCell(ts::type::Cell* cell);

    std::vector<ts::type::Cell*> getCells();
  };
}}
