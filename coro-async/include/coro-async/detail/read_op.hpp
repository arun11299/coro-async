#ifndef CORO_ASYNC_READ_OP_HPP
#define CORO_ASYNC_READ_OP_HPP

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
class read_op: public operation_base
{
public:
  ///
  template <typename Buffer>
  read_op(stream_socket& read_sock, const Buffer& buf, Handler&& ch)
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
  buffer::buffer_ref read_buffer_;
  /// The user handler to be executed
  Handler ch_;
};


/**
 */
template <typename Handler>
          // typename CompletionHandler
class composed_read_op
{
public:
  ///
  composed_read_op(stream_socket& sock,
                   //TODO: Ouch!!
                   buffer::buffer_ref& buf,
                   Handler&& h)
    : read_sock_(sock)
    , read_buffer_(buf)
    , init_bytes_(buf.size())
    , h_(std::forward<Handler>(h))
  {
  }

  // Not copyable
  composed_read_op(const composed_read_op&) = delete;

  composed_read_op(composed_read_op&& other)
    : read_sock_(other.read_sock_)
    , read_buffer_(other.read_buffer_)
    , init_bytes_(other.init_bytes_)
    , h_(std::move(other.h_))
    , bytes_consumed_(other.bytes_consumed_)
  {
  }

  // Not copy assignable
  composed_read_op& operator=(const composed_read_op&) = delete;

  composed_read_op& operator=(composed_read_op&& other)
  {
    read_sock_ = other.read_sock_;
    read_buffer_ = other.read_buffer_;
    init_bytes_ = other.init_bytes_;
    bytes_consumed_ = other.bytes_consumed_;
    h_ = std::move(other.h_);
  }

public:
  /// The state driven composed async read callback
  void operator()(const std::error_code& ec, size_t bytes_read)
  {
    if (ec && ec != error::socket_errc::eof)
    {
      // Call the final handler
      h_(ec, bytes_consumed_);
      return;
    }

    // Call the handler. Nothing more to read here.
    if (bytes_read == 0 && ec == error::socket_errc::eof)
    {
      h_(ec, bytes_consumed_);
      return;
    }

    bytes_consumed_ += bytes_read;

    if (bytes_consumed_ == init_bytes_)
    {
      // Call the handler. Our work is done
      h_(ec, bytes_consumed_);
      return;
    }
    else if (bytes_consumed_ < init_bytes_)
    {
      read_buffer_.consume(bytes_read);
    }
    else
    {
      assert (0 && "Error case");
    }

    // try to read some more data
    read_sock_.async_read_some(read_buffer_, std::move(*this));
   
    return; 
  }

private:
  /// The read stream socket
  stream_socket& read_sock_; 
  /// The read buffer
  buffer::buffer_ref& read_buffer_;
  /// Bytes intended to be read
  size_t init_bytes_ = 0;
  /// The final completion handler to be executed
  Handler h_;
  /// Total bytes read (state)
  size_t bytes_consumed_ = 0;
};

} // END namespace detail
} // END namespace coro_async

#endif
