#include <string>
#include <fstream>
#include <cassert>

#include "fragment.h"
#include <ts/system/System.h>

using ts::type::ID;

int main() {
  ts::type::FragmentTools* ct = new FragmentTools;
  ts::type::ReduceDataTools* rt = new ReduceDataTools;
  ts::system::System* system = new ts::system::System(ct, rt);

  std::vector<Fragment*> fragments;
  std::ofstream file(std::to_string(system->id()));

  fragments.push_back(new Fragment(ts::type::ID(system->id(),0,0)));
  fragments.push_back(new Fragment(ts::type::ID(system->id(),1,0)));
  fragments.push_back(new Fragment(ts::type::ID(system->id(),2,0)));
  fragments.push_back(new Fragment(ts::type::ID(system->id(),3,0)));
  fragments.push_back(new Fragment(ts::type::ID(system->id(),4,0)));

  for(auto fragment : fragments) {
    ID id = fragment->id();
    if((id.c[1] - 1) < 5)
      fragment->addNeighbour(ID(id.c[0], id.c[1] - 1, id.c[2]), id.c[0]);
    if((id.c[1] + 1) < 5)
      fragment->addNeighbour(ID(id.c[0], id.c[1] + 1, id.c[2]), id.c[0]);
    if((id.c[0] + 1) < system->size())
      fragment->addNeighbour(ID(id.c[0] + 1, id.c[1], id.c[2]), id.c[0] + 1);
    if((id.c[0] - 1) < system->size())
      fragment->addNeighbour(ID(id.c[0] - 1, id.c[1], id.c[2]), id.c[0] - 1);
  }

  for(auto fragment: fragments) {
    ID selfid = fragment->id();
    for(auto n: fragment->neighbours()) {
      file << "\"" << selfid.tostr() << "\" -> \"" << n.tostr() << "\"" << std::endl;
    }
    system->addFragment(fragment);
  }

  file.close();
  system->run();
  delete system;
  return 0;
}
