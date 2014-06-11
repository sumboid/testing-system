#pragma once
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <functional>

#include "exec/ExecMgr.h"
#include "message/MessageMgr.h"
#include "fragment/FragmentMgr.h"
#include "../types/Fragment.h"
#include "../types/ReduceDataTools.h"
#include "../types/FragmentTools.h"
#include "util/Semaphore.h"
#include "util/Listener.h"

#include "action/Action.h"
#include "action/ActionBuilder.h"


namespace ts {
namespace system {

class FragmentMgr;
class MessageMgr;
class ExecMgr;
class Action;
class ActionBuilder;

class System {
private:
  MessageMgr* msgMgr;
  FragmentMgr* fragmentMgr;
  ExecMgr* execMgr;
  ActionBuilder* actionBuilder;
  size_t inputReduceData;
  std::atomic<bool> _end;

  Semaphore fragmentListener;

  std::thread actionLoopThread;
  std::mutex queueMutex;
  std::queue<Action*> actionQueue;
  Listener actionQueueListener;

  std::thread balancerLoopThread;
  Listener balancerListener;
  std::function<std::map<ts::NodeID, double>(int, std::map<ts::NodeID, int>)> balancer;

  std::map<ts::NodeID, int> nload;
public:
  System(ts::type::FragmentTools*, ts::type::ReduceDataTools*);
  ~System();

  void addFragment(ts::type::Fragment* fragment);
  void setBalancer(std::function<std::map<ts::NodeID, double>(int, std::map<ts::NodeID, int>)> b) { balancer = b; }

  uint64_t id();
  uint64_t size();
  void run();
  void end();
  void notify();
  std::vector<ts::type::Fragment*> getFragments();

  void addAction(Action* action);
  void actionLoop();

  /// ExecMgr part
  void spreadReduceData(ts::type::ReduceData* data);
  void putReduceData(ts::type::ReduceData* data);
  void unlockFragment(ts::type::Fragment* fragment);

  int weight();
  void loadChange(NodeID node, int load);
  void balancerNotify();
  void balancerLoop();
  //put(Message* message);
};

}
}
