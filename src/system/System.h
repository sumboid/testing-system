#pragma once

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"
#include "../types/Cell.h"
#include "../types/ReduceDataTools.h"
#include "../types/CellTools.h"
#include "util/Semaphore.h"

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

  Semaphore cellListener;

public:
  System(ts::type::CellTools*, ts::type::ReduceDataTools*);
  ~System();

  void addCell(ts::type::Cell* cell);
  uint64_t id();
  uint64_t size();
  void run();
  void end() { _end = true; }
  void notify();
  std::vector<ts::type::Cell*> getCells();

  /// ExecMgr part
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);
  void unlockCell(ts::type::Cell* cell);

  //put(Message* message);
};

}
}
