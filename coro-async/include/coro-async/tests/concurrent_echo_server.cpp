#include <iostream>
#include <thread>
#include "coro_async.hpp"

using namespace coro_async;

coro_task_auto<void> handle_client(coro_socket client)
{
  char buf[6]; // for only "Hello!"
  auto bref = as_buffer(buf);
  co_await client.read(6, bref);
  co_await client.write(6, bref);
  client.close();
  co_return;
}

coro_task_auto<void> server_run(coro_acceptor& acc)
{
  while ( true )
  {
    auto client_sock = co_await acc.accept();
    handle_client(std::move(client_sock));
  }
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

  server_run(acceptor);

  std::thread thr{[&] { ios.run(); }};
  thr.join();
  return 0;
}
