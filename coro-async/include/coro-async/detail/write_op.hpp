#ifndef CORO_ASYNC_WRITE_OP_HPP
#define CORO_ASYNC_WRITE_OP_HPP

#include "coro-async/buffers.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/detail/socket_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 */
template <typename Handler>
class write_op: public operation_base
{
public:
  ///
  write_op(stream_socket& write_sock, const buffer::Buffer& buf, Handler&& ch)
    : operation_base(write_op<Handler>::complete)
    , write_sock_(write_sock)
    , write_buffer_(buf)
    , ch_(std::forward<Handler>(ch))
  {
  }

  write_op(const write_op&) = delete;
  write_op& operator=(const write_op&) = delete;

public:
  ///
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    std::cout << "write_op::complete" << std::endl;
    auto self = static_cast<write_op<Handler>*>(op);

    if (!ec)
    {
      // No error encountered yet
      std::error_code write_err{};
      size_t bytes_wrote = 0;

      bool finished = posix_socket_ops::nb_write(
                                self->write_sock_.get_native_handle(),
                                self->write_buffer_,
                                bytes_wrote,
                                write_err);
      (void)finished;

      self->ch_(write_err, bytes_wrote);
    }
    else
    {
      self->ch_(ec, 0);
    }
  }

private:
  /// The read stream socket
  stream_socket& write_sock_;
  /// The write buffer
  const buffer::Buffer& write_buffer_;
  /// The user handler to be executed
  Handler ch_;
};

} // END namespace detail
} // END namespace coro_async

#endif
