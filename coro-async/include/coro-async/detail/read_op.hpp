#ifndef CORO_ASYNC_READ_OP_HPP
#define CORO_ASYNC_READ_OP_HPP

#include "coro-async/buffers.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/detail/socket_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 */
template <typename Handler>
class read_op: public operation_base
{
public:
  ///
  read_op(stream_socket& read_sock, buffer::Buffer& buf, Handler&& ch)
    : operation_base(read_op<Handler>::complete)
    , read_sock_(read_sock)
    , read_buffer_(buf)
    , ch_(std::forward<Handler>(ch))
  {
  }

  read_op(const read_op&) = delete;
  read_op& operator=(const read_op&) = delete;

public:
  ///
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    std::cout << "read_op::complete" << std::endl;
    auto self = static_cast<read_op<Handler>*>(op);

    if (!ec)
    {
      // No error encountered yet
      std::error_code read_err{};
      size_t bytes_read = 0;

      bool finished = posix_socket_ops::nb_read(
                                self->read_sock_.get_native_handle(),
                                self->read_buffer_,
                                bytes_read,
                                read_err);
      (void)finished;

      self->ch_(read_err, bytes_read);
    }
    else
    {
      self->ch_(ec, 0);
    }
  }

private:
  /// The read stream socket
  stream_socket& read_sock_;
  /// The read buffer
  buffer::Buffer& read_buffer_;
  /// The user handler to be executed
  Handler ch_;
};

} // END namespace detail
} // END namespace coro_async

#endif
