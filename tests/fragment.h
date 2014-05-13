#include <string>
#include <fstream>
#include <cassert>

#include <ts/types/Fragment.h>
#include <ts/types/FragmentTools.h>
#include <ts/types/ID.h>
#include <ts/types/ReduceData.h>
#include <ts/types/ReduceDataTools.h>
#include <ts/util/Uberlogger.h>

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
  ts::Arc* serialize(ts::type::ReduceData* data) {
    ts::Arc* arc = new ts::Arc;
    ts::Arc& a = *arc;
    a << ((ReduceData*) data)->getNumber();
    return arc;
  }

  ts::type::ReduceData* deserialize(ts::Arc* arc) {
    char number;
    ts::Arc& a = *arc;
    a >> number;
    return new ReduceData(number);
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
        UBERLOG("fragment") << "[" << id().tostr() << "] " <<"Get iter from: " << n->id().tostr() << ": " << ((Fragment*) n)->iter << UBEREND();
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
    return fragment;
  }

  Fragment* copy() override {
    return new Fragment(ID(0,0,0));
  }
};

class FragmentTools: public ts::type::FragmentTools {
public:
  ~FragmentTools() {}
  void serialize(ts::type::Fragment* fragment, ts::Arc* arc) {
    ts::Arc& a = *arc;
    a << ((Fragment*) fragment)->iter;
  }

  ts::type::Fragment* deserialize(ts::Arc* arc) {
    ts::Arc& a = *arc;
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), false);
    a >> result->iter;
    return result;
  }

  ts::type::Fragment* createGap(const ID& id) override {
    return new Fragment(id, false);
  }
};



