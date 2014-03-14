#pragma once

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"
#include "../types/AbstractCell.h"
#include "../types/ReduceDataTools.h"
#include "../types/AbstractCellTools.h"
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

  void addCell(ts::type::AbstractCell* cell);
  int id();
  int size();
  void run();
  void end() { _end = true; }
  std::vector<ts::type::AbstractCell*> getCells();

  /// ExecMgr part
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);
  void unlockCell(ts::type::AbstractCell* cell);

  //put(Message* message);
};

}
}
