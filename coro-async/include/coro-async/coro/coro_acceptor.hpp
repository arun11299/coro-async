#ifndef CORO_ASYNC_CORO_ACCEPTOR_HPP
#define CORO_ASYNC_CORO_ACCEPTOR_HPP

#include "coro-async/endpoint.hpp"
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
  coro_acceptor(io_service& ios)
    : ios_(ios)
    , acceptor_(ios)
  {
  }

public:
  ///
  void open(const char* ip, uint16_t port, std::error_code& ec, uint32_t backlog=10)
  {
    ec.clear();

    acceptor_.open(ec);
    if (ec)
    {
      return;
    }

    endpoint ep{v4_address{ip}, port};

    acceptor_.bind(ep, ec);
    if (ec)
    {
      return;
    }

    acceptor_.listen(backlog, ec);
    if (ec)
    {
      return;
    }
  }

  ///
  // TODO: check for error
  accept_awaitable accept()
  {
    return { ios_, acceptor_ };
  }

  ///
  io_service& get_io_service() noexcept
  {
    return ios_;
  }

  ///
  tcp_acceptor& get_underlying_acceptor() noexcept
  {
    return acceptor_;
  } 

private:
  /// The io_service instance
  io_service& ios_;

  /// The acceptor instance
  tcp_acceptor acceptor_;
};

} // END namespace coro_async

#endif
