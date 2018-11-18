#ifndef CORO_ASYNC_CONNECT_AWAITABLE_HPP
#define CORO_ASYNC_CONNECT_AWAITABLE_HPP

#include <experimental/coroutine>
#include "coro-async/endpoint.hpp"
#include "coro-async/coro/coro_socket.hpp"

namespace coro_async {

/**
 */
class connect_awaitable
{
public:
  ///
  connect_awaitable(io_service& ios, endpoint ep)
    : client_sock_(ios)
    , peer_(std::move(ep))
  {
  }

  ///
  connect_awaitable(const connect_awaitable&) = delete;
  ///
  connect_awaitable& operator=(const connect_awaitable&) = delete;
  ///
  ~connect_awaitable() = default;

public: // Awaitable implementation
  ///
  bool await_ready()
  {
    return false;
  }

  ///
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    client_sock_.get_stream_sock().async_connect(peer_,
          [this, ch](const std::error_code ec, size_t bytes_xferred)
          {
            (void)bytes_xferred;
            this->handle_connection_complete(ch);
          });
  }

  ///
  coro_socket await_resume() noexcept
  {
    return std::move(client_sock_);
  }

private:
  ///
  template <typename PromiseType>
  void handle_connection_complete(stdex::coroutine_handle<PromiseType> ch)
  {
    ch.resume();
  }

private:
  /// The underlying streaming socket reference
  coro_socket client_sock_;

  /// The enpoint to connect to
  endpoint peer_;
};

} // END namespace coro_async

#endif
