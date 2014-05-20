#pragma once

#include <pthread.h>
#include <functional>

namespace ts {

class RWLock {
private:
  pthread_rwlock_t* lock;
  RWLock(const RWLock&) {}

public:
  RWLock();
  ~RWLock();

  void rlock();
  void wlock();
  void unlock();

  void rlock(std::function<void(void)> f);
  void wlock(std::function<void(void)> f);
};


}
