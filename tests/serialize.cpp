#include <iostream>
#include "fragment.h"
#include <ts/system/System.h>

using ts::type::ID;

int main() {
  FragmentTools ft;
  ts::type::Fragment* a = new Fragment(ID(1,0,0));
  char* yoba;
  size_t yobas = ft.fullSerialize(a, yoba);

  ts::type::Fragment* b = ft.fullDeserialize(yoba, yobas);
  if(a->equal(b)) std::cout << "EQUAL" << std::endl;
  else std::cout << "NOT EQUAL" << std::endl;

  delete a;
  delete b;

  return 0;
}
