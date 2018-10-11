#ifndef CORO_ASYNC_TCP_ACCEPTOR_HPP
#define CORO_ASYNC_TCP_ACCEPTOR_HPP

#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/ip_address.hpp"
#include "coro-async/endpoint.hpp"

namespace coro_async {

/**
 */
class tcp_acceptor
{
public:
  /**
   */
  tcp_acceptor(io_service& ios)
    : ios_(ios)
    , socket_(ios)
  {
  }

  tcp_acceptor(const tcp_acceptor&) = delete;
  tcp_acceptor& operator=(const tcp_acceptor&) = delete;

  ~tcp_acceptor() = default;

public:
  ///
  io_service& get_io_service()
  {
    return ios_;
  }

  ///
  bool is_open() const noexcept
  {
    return socket_.is_open();
  }

  ///
  bool open(std::error_code& ec) noexcept
  {
    ec.clear();
    return socket_.open(ec);
  }

  ///
  void bind(endpoint ep, std::error_code& ec)
  {
    socket_.bind(ep, ec);
    return;
  }

  ///
  void listen(unsigned backlog, std::error_code& ec)
  {
    socket_.listen(backlog, ec);
    return;
  }

public: // Async APIs
  /**
   */
  template <typename CompletionHandler>
  void async_accept(stream_socket& sock, CompletionHandler&& ch);

private:
  /// 
  io_service& ios_;
  ///
  stream_socket socket_;
};
 
} // END namespace coro_async

#include "coro-async/impl/tcp_acceptor.ipp"

#endif
