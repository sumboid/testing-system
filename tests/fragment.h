#include <string>
#include <fstream>
#include <cassert>

#include <ts/types/Fragment.h>
#include <ts/types/FragmentTools.h>
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

/* Fragment description */

class Fragment: public ts::type::Fragment {
friend class FragmentTools;
private:
  std::ofstream file;
  uint64_t iter;

public:
  Fragment(ts::type::ID id, bool needFile = true): ts::type::Fragment(id) {
    if(needFile) file.open(std::to_string(id.c[0]) + std::to_string(id.c[1]));
    iter = iteration();
  }

  ~Fragment() {
    file.close();
  }

  void runStep(std::vector<ts::type::Fragment*> neighbours) override {
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
        buf += ((Fragment*) n)->iter;
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

  Fragment* getBoundary() override {
    Fragment* fragment = new Fragment(id(), false);
    fragment->iter = iter;
    fragment->iteration(iteration());
    fragment->progress(progress());
    return fragment;
  }

  Fragment* copy() override {
    return new Fragment(ID(0,0,0));
  }

};

class FragmentTools: public ts::type::FragmentTools {
public:
  ~FragmentTools() {}
  void serialize(ts::type::Fragment* fragment, char*& buf, size_t& size) {
    size = 1 * sizeof(uint64_t);
    uint64_t* lbuf = new uint64_t[1];
    lbuf[0] = ((Fragment*) fragment)->iter;
    buf = reinterpret_cast<char*>(lbuf);
  }

  ts::type::Fragment* deserialize(char* buf, size_t) {
    uint64_t* lbuf = reinterpret_cast<uint64_t*>(buf);
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), false);
    result->iter = lbuf[0];
    return result;
  }

  ts::type::Fragment* createGap(const ID& id) override {
    return new Fragment(id, false);
  }
};



