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
#ifndef CORO_ASYNC_ACCEPTOR_OP_HPP
#define CORO_ASYNC_ACCEPTOR_OP_HPP

#include "coro-async/stream_socket.hpp"
#include "coro-async/detail/socket_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 * Handler for accepting a connection.
 */
template <typename Handler>
class acceptor_op: public operation_base
{
public:
  /**
   * Constructor.
   * \param new_sock - The client side of socket to be initialized.
   * \param accept_sock - The accepting socket.
   * \param ch - The completion handler to be invoked on accept.
   */
  acceptor_op(stream_socket& new_sock, stream_socket& accept_sock, Handler&& ch)
    : operation_base(acceptor_op<Handler>::complete)
    , new_sock_(new_sock)
    , accept_sock_(accept_sock)
    , ch_(std::forward<Handler>(ch))
  {
  }

  acceptor_op(const acceptor_op&) = default;
  acceptor_op& operator=(const acceptor_op&) = default;

public:
  /// Callback when socket is ready to accept a connection.
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    auto self = static_cast<acceptor_op<Handler>*>(op);
    if (!ec)
    {
      // No error encountered yet
      std::error_code acc_ec{};
      std::pair<int, endpoint> res = posix_socket_ops::accept(
                                        self->accept_sock_.get_native_handle(), acc_ec);
      if (!acc_ec)
      {
        self->new_sock_.assign(res.first, acc_ec);
        assert (self->new_sock_.is_open());
        //TODO: where to set the peer details ?
      }
      // Make the upcall to the handler
      self->ch_(acc_ec);
    }
    else
    {
      // Make the upcall with the error code
      self->ch_(ec);
    }
    return;
  }

private:
  /// The new socket to be initialized after accept
  stream_socket& new_sock_;
  /// The accepting socket reference
  stream_socket& accept_sock_;
  /// The user handler to be executed
  Handler ch_;
};

} // END namespace detail
} // END namespace coro_async

#endif
