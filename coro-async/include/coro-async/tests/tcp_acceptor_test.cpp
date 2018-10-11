#include <iostream>
#include <thread>
#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/tcp_acceptor.hpp"

using namespace coro_async;

int main() {
  io_service ios{};

  stream_socket sock{ios};
  tcp_acceptor acceptor{ios};
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

  acceptor.async_accept(sock, [](const std::error_code& ec) {
                                std::cout << "connection accepted" << std::endl;
                              });

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
