#ifndef CORO_ASYNC_WRITE_OP_HPP
#define CORO_ASYNC_WRITE_OP_HPP

#include "coro-async/buffers.hpp"
#include "coro-async/buffer_ref.hpp"
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
  template <typename Buffer>
  write_op(stream_socket& write_sock, const Buffer& buf, Handler&& ch)
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
  /// The write stream socket
  stream_socket& write_sock_;
  /// The write buffer
  const buffer::buffer_ref write_buffer_;
  /// The user handler to be executed
  Handler ch_;
};


/**
 */
template <typename Handler>
          // typename CompletionHandler
class composed_write_op
{
public:
  ///
  composed_write_op(stream_socket& sock,
                   //TODO: Ouch!!
                   const buffer::buffer_ref& buf,
                   Handler&& h)
    : write_sock_(sock)
    , write_buffer_(buf)
    , init_bytes_(buf.size())
    , h_(std::forward<Handler>(h))
  {
  }

  // Not copyable
  composed_write_op(const composed_write_op&) = delete;

  composed_write_op(composed_write_op&& other)
    : write_sock_(other.write_sock_)
    , write_buffer_(other.write_buffer_)
    , init_bytes_(other.init_bytes_)
    , h_(std::move(other.h_))
    , bytes_consumed_(other.bytes_consumed_)
  {
  }

  // Not copy assignable
  composed_write_op& operator=(const composed_write_op&) = delete;

  composed_write_op& operator=(composed_write_op&& other)
  {
    write_sock_ = other.write_sock_;
    write_buffer_ = other.write_buffer_;
    init_bytes_ = other.init_bytes_;
    bytes_consumed_ = other.bytes_consumed_;
    h_ = std::move(other.h_);
  }

  public:
  /// The state driven composed async read callback
  void operator()(const std::error_code& ec, size_t bytes_wrote)
  {
    std::cout << "composed_op::()" << std::endl;
    if (ec && ec != error::socket_errc::eof)
    {
      // Call the final handler
      h_(ec, bytes_consumed_);
      return;
    }

    // Call the handler. Nothing more to write here.
    if (bytes_wrote == 0 && ec == error::socket_errc::eof)
    {
      h_(ec, bytes_consumed_);
      return;
    }

    bytes_consumed_ += bytes_wrote;
    std::cout << init_bytes_ << " " << bytes_wrote << " " << bytes_consumed_ << std::endl;

    if (bytes_consumed_ == init_bytes_)
    {
      // Call the handler. Our work is done
      h_(ec, bytes_consumed_);
      return;
    }
    else if (bytes_consumed_ < init_bytes_)
    {
      write_buffer_.consume(bytes_wrote);
    }
    else
    {
      assert (0 && "Error case");
    }

    // try to write some more data
    write_sock_.async_write_some(write_buffer_, std::move(*this));

    return;
  }

private:
  /// The write stream socket
  stream_socket& write_sock_;
  /// The write buffer
  const buffer::buffer_ref& write_buffer_;
  /// Bytes intended to be written
  size_t init_bytes_ = 0;
  /// The final completion handler to be executed
  Handler h_;
  /// Total bytes wrote (state)
  size_t bytes_consumed_ = 0;
};

} // END namespace detail
} // END namespace coro_async

#endif
