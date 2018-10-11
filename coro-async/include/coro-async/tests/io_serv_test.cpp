#include <iostream>
#include <thread>
#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"

using namespace coro_async;

int main() {
  io_service ios{};

  stream_socket sock{ios};
  endpoint ep{v4_address{"127.0.0.1"}, 8080};
  sock.async_connect(ep, [](const std::error_code& ec, size_t bytes_xfered) {
        std::cout << "bytes xfered\n";
      });

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
