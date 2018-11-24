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

#ifndef CORO_ASYNC_CORO_ACCEPTOR_HPP
#define CORO_ASYNC_CORO_ACCEPTOR_HPP

#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/tcp_acceptor.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/coro/accept_awaitable.hpp"

namespace coro_async {

/**
 * A thin wrapper over `tcp_acceptor` to return an
 * awaitable object for performing accept call.
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
