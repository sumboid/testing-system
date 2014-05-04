#include <iostream>
#include "fragment.h"
#include <ts/system/System.h>

using ts::type::ID;

int main() {
  FragmentTools ft;
  ts::type::Fragment* a = new Fragment(ID(1,0,0));
  ts::Arc* arc = ft.fullSerialize(a);

  ts::type::Fragment* b = ft.fullDeserialize(arc);
  if(a->equal(b)) std::cout << "EQUAL" << std::endl;
  else std::cout << "NOT EQUAL" << std::endl;

  delete a;
  delete b;

  return 0;
}
