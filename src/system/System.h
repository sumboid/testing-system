#pragma once

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "cell/CellMgr.h"

namespace ts {
namespace system {

class System {
private:
  MessageMgr* msgMgr;
  CellMgr* cellMgr;
  ExecMgr* execMgr;

public:
  System();
  ~System();

  void run();

  //put(Message* message);
};

}
}
