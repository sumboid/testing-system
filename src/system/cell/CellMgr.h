#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <utility>

#include "../../types/Cell.h"
#include "../../types/CellTools.h"
#include "../message/MessageMgr.h"
#include "../System.h"

namespace ts {
namespace system {
  class System;
  class MessageMgr;

  class CellMgr {
  private:
    enum State {
      FREE,
      EXEC,
      MOVE,
      UPDATE
    };

    type::CellTools* cellTools;
    MessageMgr* messageMgr;
    System* system;

    std::vector<ts::type::Cell*> externalCells;
    std::map<ts::type::Cell*, State> cells;
    pthread_rwlock_t* cellsLock;
    pthread_rwlock_t* externalCellsLock;

    std::map<ts::type::ID, std::vector<NodeID>> movingCellAccept;

  public:
    CellMgr();
    CellMgr(MessageMgr* msgMgr);
    ~CellMgr();

    void setMessageMgr(MessageMgr* mgr) { messageMgr = mgr; }
    void setSystem(System* _system) { system = _system; }
    void setCellTools(type::CellTools* _cellTools) { cellTools = _cellTools; }

    void addCell(ts::type::Cell* cell);
    std::vector<std::pair<ts::type::Cell*, std::vector<ts::type::Cell*> > >
      getCells(int amount);
    void unlock(ts::type::Cell* cell);
    void updateExternalCell(ts::type::Cell* cell);

    void moveCell(ts::type::Cell* cell);
    void moveCellAccept(const ts::type::ID& id, NodeID nid);

    std::vector<ts::type::Cell*> getCells();
  };
}}
