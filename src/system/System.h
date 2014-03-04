#pragma once

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"
#include "../types/AbstractCell.h"

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

public:
  System(ts::type::AbstractCellTools*, ts::type::ReduceDataTools*);
  ~System();

  void run();
  void spreadReduceData(ts::type::ReduceData* data) {}

  //put(Message* message);
};

}
}
