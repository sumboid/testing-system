#include "FragmentTools.h"
#include <iostream>
#include <tuple>
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include "util/FragmentDeserializer.h"
#include "util/FragmentSerializer.h"

using std::tuple;
using std::tie;
using std::map;
using std::pair;

namespace ts {
namespace type {

using namespace util;

void FragmentTools::boundarySerialize(Fragment* fragment, char*& buf, size_t& size) {
  char* _buffer[3];
  size_t _size[3];

  serialize(fragment, _buffer[0], _size[0]);
  _size[1] = FragmentSerializer::id(fragment, _buffer[1]);
  _size[2] = FragmentSerializer::timestamp(fragment, _buffer[2]);

  size = (_size[0] + _size[1] + _size[2]);
  buf = new char[size];

  memcpy(buf,                       _buffer[0], _size[0] * sizeof(char)); //Fragment
  memcpy(buf + _size[0],            _buffer[1], _size[1] * sizeof(char)); //ID
  memcpy(buf + _size[0] + _size[1], _buffer[2], _size[2] * sizeof(char)); //Timestamp
}

Fragment* FragmentTools::boundaryDeserialize(char* buf, size_t size) {
  size_t idSize = 3 * sizeof(uint64_t) / sizeof(char);
  size_t ipSize = 2 * sizeof(uint64_t) / sizeof(char);
  size_t fragmentSize = size - idSize - ipSize;

  Fragment* fragment = deserialize(buf, fragmentSize);              // Fragment
  FragmentDeserializer::id(fragment, buf + fragmentSize, idSize); // ID
  FragmentDeserializer::timestamp(fragment, buf + fragmentSize + idSize, ipSize);

  return fragment;
}

size_t FragmentTools::fullSerialize(Fragment* fragment, char*& buf) {
  map<char*, size_t> buffers;

  size_t size = 0;
  char* localbuf;
  size_t localsize;

  serialize(fragment, localbuf, localsize);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = FragmentSerializer::id(fragment, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = FragmentSerializer::timestamp(fragment, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = FragmentSerializer::neighbours(fragment, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = FragmentSerializer::flags(fragment, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  map<char*, size_t> states;
  size_t statesSize = 0;
  size += sizeof(size_t) / sizeof(char);
  for(auto c : fragment->_vstates) {
    Fragment* stateFragment = c.second;
    stateFragment->_vid = fragment->_vid;
    tie(stateFragment->_viteration, stateFragment->_vprogress) = c.first;
    size_t stateSize = 0;
    boundarySerialize(stateFragment, localbuf, stateSize);
    states.insert(pair<char*, size_t>(localbuf, stateSize));
    statesSize++;
    size += stateSize + sizeof(size_t) / sizeof(char);
  }

  buf = new char[size];
  localsize = 0;
  for(auto i : buffers) {
    memcpy(buf + localsize, reinterpret_cast<char*>(&(i.second)), sizeof(size_t) / sizeof(char));
    localsize += sizeof(size_t) / sizeof(char);
    memcpy(buf + localsize, i.first, i.second);
    localsize += i.second;
  }

  memcpy(buf + localsize, reinterpret_cast<char*>(&(statesSize)), sizeof(size_t) / sizeof(char));
  localsize += sizeof(size_t) / sizeof(char);

  for(auto i : states) {
    memcpy(buf + localsize, reinterpret_cast<char*>(&(i.second)), sizeof(size_t) / sizeof(char));
    localsize += sizeof(size_t) / sizeof(char);
    memcpy(buf + localsize, i.first, i.second);
    localsize += i.second;
  }

  return size;
}

Fragment* FragmentTools::fullDeserialize(char* buf, size_t size) {
  Fragment* fragment;
  size_t localsize = 0;
  size_t current = 0;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  fragment = deserialize(buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  FragmentDeserializer::id(fragment, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  FragmentDeserializer::timestamp(fragment, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  FragmentDeserializer::neighbours(fragment, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  FragmentDeserializer::flags(fragment, buf + current, localsize);
  current += localsize;

  size_t statesSize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);

  for(size_t i = 0; i < statesSize; ++i) {
    localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
    current += sizeof(size_t) / sizeof(char);
    Fragment* state = boundaryDeserialize(buf + current, localsize);
    Timestamp ts = tuple<uint64_t, uint64_t>(state->_viteration, state->_vprogress);
    fragment->_vstates.insert(pair<Timestamp, Fragment*>(ts, state));
    current += localsize;
  }

  return fragment;
}
}}
