#include <string>
#include <fstream>
#include "system/System.h"
#include "types/AbstractCell.h"

class ReduceData: public ts::type::ReduceData {
private:
  char n;
public:
  ReduceData(char _n) {
    n = _n;
  }
  virtual ~ReduceData() {}

  char getNumber() { return n; }
};

class ReduceDataTools: public ts::type::ReduceDataTools {
public:
  ~ReduceDataTools() {}
  void serialize(ts::type::ReduceData* data, char*& buf, size_t& size) {
    size = 1;
    buf = new char[sizeof(char)];
    buf[0] = ((ReduceData*) data)->getNumber();
  }

  ts::type::ReduceData* deserialize(void* buf, size_t size) {
    return new ReduceData(((char*) buf)[0]);
  }

  ts::type::ReduceData* reduce(ts::type::ReduceData* d1,
                                 ts::type::ReduceData* d2) {
    return new ReduceData(((ReduceData*) d1)->getNumber() +
                            ((ReduceData*) d2)->getNumber());
  }
};

class Cell: public ts::type::AbstractCell {
  std::ofstream file;

public:
  Cell(ts::type::ID id): ts::type::AbstractCell(id),
                           file(std::to_string(id.c[0])) {
  }

  ~Cell() {
    file.close();
  }

  void run(std::vector<ts::type::AbstractCell*> neighbours) {
    file << _iteration << " iteration" << std::endl;
    _vreduce = true;
    nextIteration();
  }

  ReduceData* reduce() {
    return new ReduceData(_iteration);
  }

  ReduceData* reduce(ts::type::ReduceData* data) {
    return new ReduceData(_iteration + ((ReduceData*) data)->getNumber());
  }


  void reduceStep(ts::type::ReduceData* data) {
    file << (int) ((ReduceData*) data)->getNumber() << " reduced" << std::endl;
  }

  void update(ts::type::AbstractCell* cell) {
  }

};

class CellTools: public ts::type::AbstractCellTools {
public:
  ~CellTools() {}
  void serialize(ts::type::AbstractCell* cell, char*& buf, size_t& size) {
    buf = new char[4];
    size = 4;
  }

  ts::type::AbstractCell* deserialize(char* buf, size_t size) {
    return new Cell(ts::type::ID(0,0,0));
  }
};

int main() {
  ts::type::AbstractCellTools* ct = new CellTools;
  ts::type::ReduceDataTools* rt = new ReduceDataTools;
  ts::system::System* system = new ts::system::System(ct, rt);

  switch(system->id()) {
    case 0:
      system->addCell(new Cell(ts::type::ID(0,0,0)));
      break;
    case 1:
      system->addCell(new Cell(ts::type::ID(1,0,0)));
      break;
  }

  system->run();
  return 0;
}
