#include <iostream>
#include <thread>
#include <experimental/coroutine>
#include "coro_async.hpp"

using namespace coro_async;

coro_task_auto<void> server_connect(coro_connector& conn)
{
  auto sock = co_await conn.connect("172.217.166.110", 80);
  std::cout << "connected" << std::endl;
  (void)sock;
  co_return;
}

int main() {
  io_service ios{};
  coro_connector connector{ios};

  server_connect(connector);

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
