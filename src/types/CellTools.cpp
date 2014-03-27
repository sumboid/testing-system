#include "CellTools.h"
#include <tuple>
#include <cstring>
#include <vector>
#include <map>
#include <utility>
#include "util/CellDeserializer.h"
#include "util/CellSerializer.h"

using std::tuple;
using std::tie;
using std::map;
using std::pair;

namespace ts {
namespace type {

using namespace util;

void CellTools::boundarySerialize(Cell* cell, char*& buf, size_t& size) {
  char* _buffer[3];
  size_t _size[3];

  serialize(cell, _buffer[0], _size[0]);
  _size[1] = CellSerializer::id(cell, buf);
  _size[2] = CellSerializer::timestamp(cell, buf);

  size = (_size[0] + _size[1] + _size[2]);
  buf = new char[size];


  memcpy(buf, _buffer[0], _size[0] * sizeof(char));                       //Cell
  memcpy(buf + _size[0], _buffer[1], _size[1] * sizeof(char));            //ID
  memcpy(buf + _size[0] + _size[1], _buffer[2], _size[2] * sizeof(char)); //Timestamp
}

Cell* CellTools::boundaryDeserialize(char* buf, size_t size) {
  size_t idSize = 3 * sizeof(uint64_t) / sizeof(char);
  size_t ipSize = 2 * sizeof(uint64_t) / sizeof(char);
  size_t cellSize = size - idSize - ipSize;

  Cell* cell = deserialize(buf, cellSize);              // Cell
  CellDeserializer::id(cell, buf + cellSize, idSize); // ID
  CellDeserializer::timestamp(cell, buf + cellSize + idSize, ipSize);

  return cell;
}

size_t CellTools::fullSerialize(Cell* cell, char*& buf) {
  map<char*, size_t> buffers;

  size_t size = 0;
  char* localbuf;
  size_t localsize;

  serialize(cell, localbuf, localsize);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = CellSerializer::id(cell, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = CellSerializer::timestamp(cell, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = CellSerializer::neighbours(cell, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  localsize = CellSerializer::flags(cell, localbuf);
  buffers.insert(pair<char*, size_t>(localbuf, localsize));
  size += localsize + sizeof(size_t) / sizeof(char);

  map<char*, size_t> states;
  size_t statesSize = 0;
  size += sizeof(size_t) / sizeof(char);
  for(auto c : cell->_vstates) {
    Cell* stateCell = c.second;
    stateCell->_vid = cell->_vid;
    tie(stateCell->_viteration, stateCell->_vprogress) = c.first;
    size_t stateSize = 0;
    boundarySerialize(stateCell, localbuf, stateSize);
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

Cell* CellTools::fullDeserialize(char* buf, size_t size) {
  Cell* cell;
  size_t localsize = 0;
  size_t current = 0;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  cell = deserialize(buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  CellDeserializer::id(cell, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  CellDeserializer::timestamp(cell, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  CellDeserializer::neighbours(cell, buf + current, localsize);
  current += localsize;

  localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);
  CellDeserializer::flags(cell, buf + current, localsize);
  current += localsize;

  size_t statesSize = reinterpret_cast<uint64_t*>(buf + current)[0];
  current += sizeof(size_t) / sizeof(char);

  for(size_t i = 0; i < statesSize; ++i) {
    localsize = reinterpret_cast<uint64_t*>(buf + current)[0];
    current += sizeof(size_t) / sizeof(char);
    Cell* state = boundaryDeserialize(buf + current, localsize);
    Timestamp ts = tuple<uint64_t, uint64_t>(state->_viteration, state->_vprogress);
    cell->_vstates.insert(pair<Timestamp, Cell*>(ts, state));
    current += localsize;
  }

  return cell;
}
}}
