#include "Listener.h"

ts::system::Listener::Listener(bool begin) {
  condition.store(begin);
}

void ts::system::Listener::wait() {
  std::unique_lock<std::mutex> lock(mutex);
  if(!condition) {
    listener.wait(lock, [=](){ bool _ = condition.load(); return _; });
  }
  condition = false;
  lock.unlock();
}

void ts::system::Listener::notifyAll() {
  std::lock_guard<std::mutex> lock(mutex);
  condition.store(true);
  listener.notify_all();
}

void ts::system::Listener::notifyOne() {
  std::lock_guard<std::mutex> lock(mutex);
  condition.store(true);
  listener.notify_one();
}

void ts::system::Listener::invert() {
  condition.store(!condition.load());
}
