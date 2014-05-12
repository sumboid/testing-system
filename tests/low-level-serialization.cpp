#include <iostream>
#include <cstdint>
#include <ts/util/Arc.h>

int main() {
  char     chars[]   = {   52,  14, -15 };
  int      ints[]    = { 1086, 359,  85 };
  uint64_t uint64s[] = { 1245, 123, 124 };

  char rchars[3];
  int  rints[3];
  uint64_t ruint64s[3];

  ts::Arc arc;

  std::cout << " ---  BEFORE  --- " << std::endl;
  for(int i = 0; i < 3; ++i) {
    std::cout << (int)chars[i] << " " << ints[i] << " " << uint64s[i] << std::endl;
    arc << chars[i] << ints[i] << uint64s[i];
  }

  std::cout << " ---   AFTER  --- " << std::endl;

  for(int i = 0; i < 3; ++i) {
    arc >> rchars[i] >> rints[i] >> ruint64s[i];
    std::cout << (int)rchars[i] << " " << rints[i] << " " << ruint64s[i] << std::endl;
  }

  for(int i = 0; i < 3; ++i)
    if(chars[i]   != rchars[i] ||
       ints[i]    != rints[i]  ||
       uint64s[i] != ruint64s[i]) {
      std::cout << "FAIL" << std::endl;
      return 0;
    }

  std::cout << "SUCCESS" << std::endl;
  return 0;
}
