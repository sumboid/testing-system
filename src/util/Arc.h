#pragma once
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <type_traits>

namespace ts {

class Arc {
private:
  char* _raw;
  size_t _size;
  size_t wpos;
  size_t _rpos;

public:
  Arc(const char* data, size_t __size) {
    _raw = (char*) malloc(__size * sizeof(char));
    memcpy(_raw, data, __size * sizeof(char));
    _size = __size;
    _rpos = 0;
  }

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
      std::cout << "D'OH!" << std::endl;
    }

    memcpy(_raw + _size, rdata, s);
    _size += s;

    return *this;
  }

  template<class T>
  Arc& operator>> (T& something) {
    static_assert(std::is_fundamental<T>::value, "Arc can process primitive types only");
    size_t s = sizeof(T) / sizeof(char);
    char data[s];

    if(s > _size - _rpos) {
      std::cout << "Oh god" << std::endl;
    }

    memcpy(data, _raw + _rpos, s * sizeof(char));
    _rpos += s;

    something = *(reinterpret_cast<T*>(data));

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
};
}
