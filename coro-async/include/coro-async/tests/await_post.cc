#include <thread>
#include <iostream>
#include "coro-async/coro/coro_task.hpp"
#include "coro-async/coro_scheduler.hpp"
#include "coro-async/io_service.hpp"

using namespace coro_async;
using namespace std::chrono_literals;

coro_task_auto<void> waiter(io_service& ios)
{
  coro_scheduler s{ios};
  std::cout << "Wait for completion\n";
  co_await s.wait_for(
        [&ios]() -> coro_task_auto<void> {
          std::cout << "called again??\n";
          coro_scheduler s{ios};
          std::cout << "start sleep" << std::endl;
          co_await s.yield_for(2s);
          std::cout << "end sleep" << std::endl;
          co_return;
        }
      );
  std::cout << "Wait completed\n";
  co_return;
}

int main() {
  io_service ios{};
  waiter(ios);
  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
