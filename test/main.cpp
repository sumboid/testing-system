#include <string>
#include <fstream>
#include <cassert>

#include <ts/system/System.h>

#include <ts/types/Cell.h>
#include <ts/types/CellTools.h>
#include <ts/types/ID.h>
#include <ts/types/ReduceData.h>
#include <ts/types/ReduceDataTools.h>

using ts::type::ID;
class ReduceData: public ts::type::ReduceData {
private:
  char n;
public:
  ReduceData(char _n) {
    n = _n;
  }
  virtual ~ReduceData() {}

  char getNumber() { return n; }
  virtual ts::type::ReduceData* copy() { return new ReduceData(n); }
};

class ReduceDataTools: public ts::type::ReduceDataTools {
public:
  ~ReduceDataTools() {}
  void serialize(ts::type::ReduceData* data, char*& buf, size_t& size) {
    size = 1;
    buf = new char[1];
    buf[0] = ((ReduceData*) data)->getNumber();
  }

  ts::type::ReduceData* deserialize(void* buf, size_t) {
    return new ReduceData(((char*) buf)[0]);
  }

  ts::type::ReduceData* reduce(ts::type::ReduceData* d1,
                                 ts::type::ReduceData* d2) {
    return new ReduceData(((ReduceData*) d1)->getNumber() +
                            ((ReduceData*) d2)->getNumber());
  }
};

/* Cell description */

class Cell: public ts::type::Cell {
friend class CellTools;
private:
  std::ofstream file;
  uint64_t iter;

public:
  Cell(ts::type::ID id, bool needFile = true): ts::type::Cell(id) {
    if(needFile) file.open(std::to_string(id.c[0]) + std::to_string(id.c[1]));
    iter = iteration();
  }

  ~Cell() {
    file.close();
  }

  void runStep(std::vector<ts::type::Cell*> neighbours) override {
    if(iteration() == 10) {
      setEnd();
      return;
    }

    file << iteration() << " iteration" << std::endl;
    iter = iteration();
    if(iteration() % 2 == 1) {
      saveState();
      setUpdate();
      setNeighbours(iteration(), progress());
    } else if (iteration() != 0) {
      int buf = iter - 1;
      for(auto n: neighbours) {
        buf += ((Cell*) n)->iter;
      }
      file << "Reduced " << buf << " locally" << std::endl;
    }
    nextIteration();
  }

  ReduceData* reduce() override {
    return new ReduceData(iteration());
  }

  ReduceData* reduce(ts::type::ReduceData* data) override {
    return new ReduceData(iteration() + ((ReduceData*) data)->getNumber());
  }


  void reduceStep(ts::type::ReduceData* data) override {
    file << (int) ((ReduceData*) data)->getNumber() << " reduced" << std::endl;
  }

  Cell* getBoundary() override {
    Cell* cell = new Cell(id(), false);
    cell->iter = iter;
    cell->iteration(iteration());
    cell->progress(progress());
    return cell;
  }

};

class CellTools: public ts::type::CellTools {
public:
  ~CellTools() {}
  void serialize(ts::type::Cell* cell, char*& buf, size_t& size) {
    size = 1 * sizeof(uint64_t);
    uint64_t* lbuf = new uint64_t[1];
    lbuf[0] = ((Cell*) cell)->iter;
    buf = reinterpret_cast<char*>(lbuf);
  }

  ts::type::Cell* deserialize(char* buf, size_t) {
    uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
    Cell* result = new Cell(ts::type::ID(0, 0, 0), false);
    result->iter = lbuf[0];
    return result;
  }

  ts::type::Cell* createGap(const ID& id) override {
    return new Cell(id, false);
  }
};

int main() {
  ts::type::CellTools* ct = new CellTools;
  ts::type::ReduceDataTools* rt = new ReduceDataTools;
  ts::system::System* system = new ts::system::System(ct, rt);

  std::vector<Cell*> cells;
  std::ofstream file(std::to_string(system->id()));

  cells.push_back(new Cell(ts::type::ID(system->id(),0,0)));
  cells.push_back(new Cell(ts::type::ID(system->id(),1,0)));
  cells.push_back(new Cell(ts::type::ID(system->id(),2,0)));
  cells.push_back(new Cell(ts::type::ID(system->id(),3,0)));
  cells.push_back(new Cell(ts::type::ID(system->id(),4,0)));

  for(auto cell : cells) {
    ID id = cell->id();
    if((id.c[1] - 1) < 5)
      cell->updateNeighbour(ID(id.c[0], id.c[1] - 1, id.c[2]), id.c[0]);
    if((id.c[1] + 1) < 5)
      cell->updateNeighbour(ID(id.c[0], id.c[1] + 1, id.c[2]), id.c[0]);
    if((id.c[0] + 1) < system->size())
      cell->updateNeighbour(ID(id.c[0] + 1, id.c[1], id.c[2]), id.c[0] + 1);
    if((id.c[0] - 1) < system->size())
      cell->updateNeighbour(ID(id.c[0] - 1, id.c[1], id.c[2]), id.c[0] - 1);
  }

  for(auto cell: cells) {
    ID selfid = cell->id();
    for(auto n: cell->neighbours()) {
      file << "\"" << selfid.tostr() << "\" -> \"" << n.tostr() << "\"" << std::endl;
    }
    system->addCell(cell);
  }

  file.close();
  system->run();
  delete system;
  return 0;
}
