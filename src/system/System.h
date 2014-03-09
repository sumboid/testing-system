#pragma once

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"
#include "../types/AbstractCell.h"
#include "util/Listener.h"

namespace ts {
namespace system {

class CellMgr;
class MessageMgr;
class ExecMgr;

class System {
private:
  MessageMgr* msgMgr;
  CellMgr* cellMgr;
  ExecMgr* execMgr;

  size_t inputReduceData;
  bool _end;

  Listener cellListener;

public:
  System(ts::type::AbstractCellTools*, ts::type::ReduceDataTools*);
  ~System();

  void addCell(ts::type::AbstractCell* cell); // { cellMgr->addCell(cell); }
  int id(); // { return msgMgr->getNodeID(); }
  int size(); // { return msgMgr->getNodeID(); }
  void run();
  void end() { _end = true; }

  /// ExecMgr part
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);
  void unlockCell(ts::type::AbstractCell* cell);

  //put(Message* message);
};

}
}
