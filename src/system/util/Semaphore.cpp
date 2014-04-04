#include "Semaphore.h"

ts::system::Semaphore::Semaphore(int begin) {
  value.store(begin);
}

void ts::system::Semaphore::wait() {
  std::unique_lock<std::mutex> lock(mutex);
  if(value.load() == 0) {
    listener.wait(lock, [=](){ int _ = value.load(); return _ > 0; });
  }
  --value;
  lock.unlock();
}

void ts::system::Semaphore::notifyAll() {
  std::lock_guard<std::mutex> lock(mutex);
  ++value;
  listener.notify_all();
}

void ts::system::Semaphore::notifyOne() {
  std::lock_guard<std::mutex> lock(mutex);
  ++value;
  listener.notify_one();
}
