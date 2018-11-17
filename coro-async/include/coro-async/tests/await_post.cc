#include <thread>
#include <iostream>
#include "coro-async/coro/coro_task.hpp"
#include "coro-async/coro_scheduler.hpp"
#include "coro-async/io_service.hpp"

using namespace coro_async;
using namespace std::chrono_literals;

coro_task_auto<void> sleeper(io_service& ios)
{
  coro_scheduler s{ios};
  std::cout << "start sleep" << std::endl;
  co_await s.yield_for(2s);
  std::cout << "end sleep" << std::endl;
  co_return;
}

coro_task_auto<int> deep_thinking(io_service& ios)
{
  coro_scheduler s{ios};
  co_await s.yield_for(2s);
  co_return 42;
}

coro_task_auto<void> waiter(io_service& ios)
{
  coro_scheduler s{ios};
  std::cout << "Wait for completion" << std::endl;
  co_await s.wait_for([&ios] () -> auto { return sleeper(ios); });
  std::cout << "Wait completed" << std::endl;

  std::cout << "Do some deep thinking" << std::endl;
  auto result = co_await s.wait_for([&ios]() -> auto { return deep_thinking(ios); });
  std::cout << "Answer after deep thinking: " << result.result() << std::endl;
  co_return;
}

int main() {
  io_service ios{};
  waiter(ios);
  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
