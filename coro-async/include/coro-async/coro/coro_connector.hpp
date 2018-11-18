#ifndef CORO_ASYNC_CORO_CONNECTOR_HPP
#define CORO_ASYNC_CORO_CONNECTOR_HPP

#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/coro/connect_awaitable.hpp"

namespace coro_async {

/**
 */
class coro_connector
{
public:
  ///
  coro_connector(io_service& ios)
    : ios_(ios)
  {
  }

public:
  ///
  // TODO: check for error
  connect_awaitable connect(const char* ip, uint16_t port)
  {
    endpoint ep{v4_address{ip}, port};
    return { ios_, ep };
  }

  ///
  io_service& get_io_service() noexcept
  {
    return ios_;
  }

private:
  /// The io_service instance
  io_service& ios_;
};

} // END namespace coro_async

#endif
