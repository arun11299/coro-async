#ifndef CORO_ASYNC_ACCEPT_AWAITABLE_HPP
#define CORO_ASYNC_ACCEPT_AWAITABLE_HPP

#include <experimental/coroutine>

#include "coro-async/tcp_acceptor.hpp"
#include "coro-async/stream_socket.hpp"

namespace coro_async {

/**
 */
class accept_awaitable
{
public:
  ///
  accept_awaitable(tcp_acceptor& acceptor, stream_socket& sock)
    : acceptor_(acceptor)
    , sock_(sock)
  {
    std::cout << "cons" << std::endl;
  }

  ///
  accept_awaitable(const accept_awaitable&) = delete;
  ///
  accept_awaitable& operator=(const accept_awaitable&) = delete;
  ///
  ~accept_awaitable()
  {
    std::cout << "dest" << std::endl;
  }

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
    acceptor_.async_accept(sock_, [this, ch](const std::error_code ec) {
                                    std::cout << "connection accepted" << std::endl;
                                    this->handle_connection_accept(ch);
                                  }
                          );   
  }

  void await_resume() noexcept
  {
  }

private:
  ///
  template <typename PromiseType>
  void handle_connection_accept(stdex::coroutine_handle<PromiseType> ch)
  {
    ch.resume();
  }

private:
  /// The acceptor
  tcp_acceptor& acceptor_;

  /// The underlying streaming socket reference
  stream_socket& sock_;
};

} // END namespace coro_async

#endif
