#pragma once
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace ts {

class Arc {
private:
  std::stringstream raw;

public:
  Arc(const char* data, size_t size) {
    raw.write(data, size);
  }

  Arc() {}
  ~Arc() {}

  template<class T>
  Arc& operator<< (const T& something) {
    size_t size = sizeof(T) / sizeof(char);

    T data = something;
    char* rdata = reinterpret_cast<char*>(&data);

    raw.write(rdata, size);
    return *this;
  }

  template<class T>
  Arc& operator>> (T& something) {
    size_t size = sizeof(T) / sizeof(char);
    char data[size];

    raw.read(data, size);
    something = *(reinterpret_cast<T*>(data));

    return *this;
  }

  size_t size() {
    raw.seekg(0, raw.end);
    size_t size = raw.tellg();
    raw.seekg(0, raw.beg);
    return size;
  }

  char* get() {
    size_t s = size();
    char* r = new char[s];
    memcpy(r, raw.str().c_str(), s);
    return r;
  }
};
}
