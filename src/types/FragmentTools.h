#pragma once
#include <cstdlib>
#include "Fragment.h"

namespace ts {
namespace type {
class Fragment;
class FragmentTools {
public:
  virtual ~FragmentTools() {}
  virtual void serialize(Fragment* fragment, char*& buf, size_t& size) = 0;
  virtual Fragment* deserialize(char* buf, size_t size) = 0;
  virtual Fragment* createGap(const ID& id) = 0;

  size_t fullSerialize(Fragment* fragment, char*& buf);
  void boundarySerialize(Fragment* fragment, char*& buf, size_t& size);

  Fragment* fullDeserialize(char* buf, size_t size);
  Fragment* boundaryDeserialize(char* buf, size_t size);
};



}}
