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

  size_t inputReduceData;

public:
  System(ts::type::AbstractCellTools*, ts::type::ReduceDataTools*);
  ~System();

  void addCell(ts::type::AbstractCell* cell); // { cellMgr->addCell(cell); }
  int id(); // { return msgMgr->getNodeID(); }
  int size(); // { return msgMgr->getNodeID(); }
  void run();
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);

  //put(Message* message);
};

}
}
