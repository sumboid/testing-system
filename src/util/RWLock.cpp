#include "RWLock.h"

namespace ts {

RWLock::RWLock() {
  lock = new pthread_rwlock_t;
  pthread_rwlock_init(lock, 0);
}

RWLock::~RWLock() {
  pthread_rwlock_destroy(lock);
  delete lock;
}

void RWLock::rlock() {
  pthread_rwlock_rdlock(lock);
}

void RWLock::wlock() {
  pthread_rwlock_wrlock(lock);
}

void RWLock::unlock() {
  pthread_rwlock_unlock(lock);
}

void RWLock::rlock(std::function<void(void)> f) {
  rlock();
  f();
  unlock();
}

void RWLock::wlock(std::function<void(void)> f) {
  wlock();
  f();
  unlock();
}

}
