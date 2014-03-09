#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace ts {
namespace system {

struct Listener {
  std::condition_variable listener;
  std::mutex mutex;
  std::atomic<bool> condition;

  Listener(bool begin = false);
  void wait();
  void notifyAll();
  void notifyOne();
  void invert();
};

}}
