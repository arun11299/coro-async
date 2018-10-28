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
  auto client_sock = co_await acc.accept();
  char buf[4];
  auto bref = as_buffer(buf);

  co_await client_sock.read(4, bref);
  std::cout << "Read from nw: " << std::string{buf} << std::endl;

  co_await client_sock.write(4, bref);

  co_return;
}

int main() {
  io_service ios{};
  coro_acceptor acceptor{ios};

  std::error_code ec{};
  acceptor.open("127.0.0.1", 8080, ec);

  if (ec)
  {
    std::cout << "error: " << ec.message() << std::endl;
    return -1;
  }

  accept_conn(acceptor);

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
