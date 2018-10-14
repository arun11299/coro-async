#include <iostream>
#include <thread>
#include "coro-async/buffers.hpp"
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

  buffer::Buffer buf;
  // Only need to read 4 bytes of data
  buf.resize(10);
  auto bref = coro_async::as_buffer(buf);


  acceptor.async_accept(sock, [&sock, &bref, &buf](const std::error_code& ec) {
                                std::cout << "connection accepted" << std::endl;
                                // Read some data
                                sock.async_read(bref, [&buf](std::error_code rec, size_t bytes) {
                                      std::cout << "Read data" << std::endl;
                                      std::cout << buffer::helpers::to_string(buf) << std::endl;
                                    });
                              });

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
