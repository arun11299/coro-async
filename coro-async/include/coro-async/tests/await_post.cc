#include <thread>
#include <iostream>
#include "coro-async/coro/coro_task.hpp"
#include "coro-async/coro_scheduler.hpp"
#include "coro-async/io_service.hpp"

using namespace coro_async;
using namespace std::chrono_literals;

coro_task<void> waiter(io_service& ios)
{
  coro_scheduler s{ios};
  co_await s.wait_for([]() { std::cout << "called" << std::endl; } );
  co_return;
}

int main() {
  io_service ios{};
  waiter(ios);
  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
