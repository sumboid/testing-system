#pragma once

#include <pthread.h>

#include <vector>
#include <map>
#include <utility>

#include "../../types/Fragment.h"
#include "../../types/FragmentTools.h"
#include "../message/MessageMgr.h"
#include "../System.h"
#include "../message/NodeID.h"
#include "../../util/RWLock.h"

namespace ts {
namespace system {

  class System;
  class MessageMgr;

  class FragmentMgr {
  private:
    enum State {
      FREE,
      EXEC,
      MOVE,
      UPDATE,
      BLOCKED
    };

    typedef std::map<ts::type::Fragment*, State>::iterator internalit;
    typedef std::vector<ts::type::Fragment*>::iterator externalit;

    type::FragmentTools* fragmentTools;
    MessageMgr* messageMgr;
    System* system;

    std::vector<ts::type::Fragment*> externalFragments;
    std::map<ts::type::Fragment*, State> fragments;
    ts::RWLock fragmentsLock;
    ts::RWLock externalFragmentsLock;

    std::map<ts::type::ID, std::vector<NodeID>> movingFragmentAccept;
    std::map<ts::type::ID, NodeID> moveList;

  public:
    FragmentMgr();
    FragmentMgr(MessageMgr* msgMgr);
    ~FragmentMgr();

    void setMessageMgr(MessageMgr* mgr) { messageMgr = mgr; }
    void setSystem(System* _system) { system = _system; }
    void setFragmentTools(type::FragmentTools* _fragmentTools) { fragmentTools = _fragmentTools; }

    void addFragment(ts::type::Fragment* fragment);
    std::vector<std::pair<ts::type::Fragment*, std::vector<ts::type::Fragment*> > >
      getFragments(int amount);
    void unlock(ts::type::Fragment* fragment);
    void updateExternalFragment(ts::type::Fragment* fragment);

    void specialUpdateNeighbours(ts::type::Fragment* fragment);
    void confirmMove(const ts::type::ID& id, NodeID node);
    void updateNeighbours(const ts::type::ID& id, NodeID node);
    void startMoveFragment(ts::type::Fragment* fragment, NodeID node);
    void moveFragment(ts::type::Fragment* fragment);
    void moveFragmentAccept(const ts::type::ID& id, NodeID nid);
    void createExternal(ts::type::Fragment* f);
    std::vector<ts::type::Fragment*> getFragments();
    ts::type::Fragment* findFragment(const ts::type::ID& id);

    internalit findInternalFragment(const ts::type::ID& id);
    externalit findExternalFragment(const ts::type::ID& id);
  };
}}
