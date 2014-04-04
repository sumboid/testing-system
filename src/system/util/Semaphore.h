#pragma once

#include <mutex>
#include <atomic>
#include <condition_variable>

namespace ts {
namespace system {

struct Semaphore {
  std::condition_variable listener;
  std::mutex mutex;
  std::atomic<int> value;

  Semaphore(int begin = 0);
  void wait();
  void notifyAll();
  void notifyOne();
};

}}
