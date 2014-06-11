#pragma once
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <type_traits>
#include <vector>

#include "Uberlogger.h"

namespace ts {

class Arc {
private:
  char* _raw;
  size_t _size;
  size_t _rpos;

  std::vector<size_t> vput;
  std::vector<size_t> vget;

public:
  Arc(const char* data, size_t __size): _size(__size), _rpos(0) {
    _raw = (char*) malloc(__size * sizeof(char));
    memcpy(_raw, data, __size * sizeof(char));
    vput.push_back(__size);
  }

  Arc(const Arc& other): Arc(other._raw, other._size) {}

  Arc() {
    _raw = 0;
    _size = 0;
    _rpos = 0;
  }

  ~Arc() {
    if(_raw != 0)
      free(_raw);
  }

  template<class T>
  Arc& operator<< (const T& something) {
    static_assert(std::is_fundamental<T>::value, "Arc can process primitive types only");
    size_t s = sizeof(T) / sizeof(char);

    T data = something;
    char* rdata = reinterpret_cast<char*>(&data);

    char* _newraw = (char*) realloc(_raw, (_size + s) * sizeof(char));
    if(_newraw != 0) {
      _raw = _newraw;
    } else {
      ULOG(error) << "D'OH!" << UEND;
    }

    memcpy(_raw + _size, rdata, s);
    _size += s;

    vput.push_back(s);
    return *this;
  }

  template<class T>
  Arc& operator>> (T& something) {
    static_assert(std::is_fundamental<T>::value, "Arc can process primitive types only");
    size_t s = sizeof(T) / sizeof(char);
    char data[s];

    vget.push_back(s);

    if(s > _size - _rpos) {
      auto message = ULOG(error);
      message << "(s > _size - _rpos) or (" << s << " > " << _size - _rpos  << ")" <<
                 " and _size = " << _size << "\n";

      message << "put: ";
      for(auto i : vput)
        message << i << ", ";

      message << "\n";
      message << "get: ";
      for(auto i : vget)
        message << i << ", ";

      message << UEND;
      assert(0);
    }

    memcpy(&data, _raw + _rpos, s * sizeof(char));
    _rpos += s;

    union {
      char* c;
      T* i;
    } yoba;

    yoba.c = data;
    something = *(yoba.i);

    return *this;
  }

  size_t size() {
    return _size;
  }

  char* get() {
    char* r = new char[_size];
    memcpy(r, _raw, _size);
    return r;
  }

  size_t pos() {
    return _rpos;
  }
};
}
