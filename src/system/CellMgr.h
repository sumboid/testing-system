#pragma once

#include <vector>
#include <map>

namespace ts {
namespace system {
    class CellMgr {
        private:
            MessageMgr* messageMgr;
            std::vector<int> neighbours;
            std::map<Cell*, > cells;
        public:

            

            void setMessageMgr(MessageMgr*);
            void addNeighbour(int);
    };
}
}
