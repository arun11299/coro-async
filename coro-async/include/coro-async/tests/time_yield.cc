#include <thread>
#include <iostream>
#include "coro-async/coro/coro_task.hpp"
#include "coro-async/scheduler.hpp"
#include "coro-async/io_service.hpp"

using namespace coro_async;
using namespace std::chrono_literals;

coro_task<void> waiter(io_service& ios)
{
  scheduler s{ios};
  std::cout << "start sleep" << std::endl;
  co_await s.yield_for(2s);
  std::cout << "end sleep" << std::endl;
  co_return;
}

int main() {
  io_service ios{};
  waiter(ios);
  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
