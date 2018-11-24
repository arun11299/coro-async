/*
  Copyright (c) 2018 Arun Muralidharan

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef CORO_ASYNC_ACCEPT_AWAITABLE_HPP
#define CORO_ASYNC_ACCEPT_AWAITABLE_HPP

#include <experimental/coroutine>

#include "coro-async/tcp_acceptor.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/coro/coro_socket.hpp"

namespace coro_async {

/**
 * An awaitable for performing socket accept call.
 */
class accept_awaitable
{
public:
  ///
  accept_awaitable(io_service& ios, tcp_acceptor& acceptor)
    : acceptor_(acceptor)
    , client_sock_(ios)
  {
  }

  ///
  accept_awaitable(const accept_awaitable&) = delete;
  ///
  accept_awaitable& operator=(const accept_awaitable&) = delete;
  ///
  ~accept_awaitable()
  {
  }

public: // Awaitable implementation
  ///
  bool await_ready()
  {
    return false;
  }

  /**
   * Performs an async_accept with completion handler 
   * doing the coroutine resume.
   */
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    acceptor_.async_accept(client_sock_.get_stream_sock(), 
          [this, ch](const std::error_code ec) {
            acc_ec_ = ec;
            this->handle_connection_accept(ch);
          });
  }

  /**
   * Returns the value of the awaited operation.
   * Return type is `result_type_non_coro` as the handler
   * is not a coroutine.
   */
  result_type_non_coro<coro_socket> await_resume() noexcept
  {
    if (acc_ec_)
    {
      return { acc_ec_ };
    }
    return std::move(client_sock_);
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
  coro_socket client_sock_;

  /// Error code from async accept
  std::error_code acc_ec_;
};

} // END namespace coro_async

#endif
