#include "Listener.h"

ts::system::Listener::Listener(bool begin) {
  condition = begin;
}

void ts::system::Listener::wait() {
  std::unique_lock<std::mutex> lock(mutex);
  if(!condition) {
    listener.wait(lock, [=](){ bool _ = condition; return _; });
  }
  condition = false;
  lock.unlock();
}

void ts::system::Listener::notifyAll() {
  std::lock_guard<std::mutex> lock(mutex);
  condition = true;
  listener.notify_all();
}

void ts::system::Listener::notifyOne() {
  std::lock_guard<std::mutex> lock(mutex);
  condition = true;
  listener.notify_one();
}
