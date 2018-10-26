#ifndef CORO_ASYNC_CORO_ACCEPTOR_HPP
#define CORO_ASYNC_CORO_ACCEPTOR_HPP

#include "coro-async/io_service.hpp"
#include "coro-async/tcp_acceptor.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/coro/accept_awaitable.hpp"

namespace coro_async {

/**
 */
class coro_acceptor
{
public:
  ///
  coro_acceptor(io_service& ios, tcp_acceptor& acceptor)
    : ios_(ios)
    , acceptor_(acceptor)
    , client_sock_(ios)
    , awaitable_(acceptor_, client_sock_)
  {
  }

public:
  ///
  accept_awaitable& accept()
  {
    return awaitable_;
  }

private:
  /// The io_service instance
  io_service& ios_;

  /// The acceptor instance
  tcp_acceptor& acceptor_;

  /// The client socket
  stream_socket client_sock_;

  /// The awaitable object instance
  accept_awaitable awaitable_;
};

} // END namespace coro_async

#endif
