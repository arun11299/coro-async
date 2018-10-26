#include <iostream>
#include <thread>
#include <experimental/coroutine>
#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/tcp_acceptor.hpp"
#include "coro-async/coro/coro_task.hpp"
#include "coro-async/coro/coro_acceptor.hpp"

using namespace coro_async;

coro_task<int> accept_conn(coro_acceptor& acc)
{
  co_await acc.accept();
  co_return;
}

int main() {
  io_service ios{};
  tcp_acceptor acceptor{ios};
  coro_acceptor cacceptor{ios, acceptor};
  std::error_code ec{};

  endpoint ep{v4_address{"127.0.0.1"}, 8080};
  acceptor.open(ec);
  if (ec) {
    std::cout << "acceptor init failed: " << ec.message() << '\n';
    return -1;
  }
  acceptor.bind(ep, ec);
  if (ec) {
    std::cout << "bind failed: " << ec.message() << '\n';
    return -1;
  }
  acceptor.listen(1, ec);
  if (ec) {
    std::cout << "listen failed: " << ec.message() << '\n';
    return -1;
  }

  accept_conn(cacceptor);

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
