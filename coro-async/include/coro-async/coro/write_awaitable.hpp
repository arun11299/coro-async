#ifndef CORO_ASYNC_WRITE_AWAITABLE_HPP
#define CORO_ASYNC_WRITE_AWAITABLE_HPP

#include "coro-async/coro/result.hpp"

namespace coro_async {

/**
 */
class write_awaitable
{
public:
  ///
  write_awaitable(stream_socket& sock, size_t write_bytes, buffer::buffer_ref buf)
    : sock_(sock)
    , bytes_to_write_(write_bytes)
    , write_buf_(buf)
  {
    ec_.clear();
  }

  ///
  write_awaitable(const write_awaitable&) = delete;
  ///
  write_awaitable& operator=(const write_awaitable&) = delete;
  ///
  ~write_awaitable() = default;

public: // Awaitable Implementation
  ///
  bool await_ready()
  {
    //TODO: socket write optimizations
    return false;
  }

  ///
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    sock_.async_write(
        write_buf_, [this, ch](const std::error_code ec, const size_t wr_bytes) {
                      this->handle_write_cb(ch, ec, wr_bytes);
                      return;
                    }
       );
  }

  ///
  result_type<size_t> await_resume()
  {
    if (ec_) return { ec_ };
    else     return { bytes_wrote_ };
  }

private:
  ///
  template <typename PromiseType>
  void handle_write_cb(stdex::coroutine_handle<PromiseType> ch,
                       const std::error_code ec,
                       const size_t wr_bytes)
  {
    if (ec) ec_ = ec;
    else bytes_wrote_ = wr_bytes;

    ch.resume();
  }

private:
  ///
  stream_socket& sock_;

  ///
  const size_t bytes_to_write_ = 0;

  ///
  buffer::buffer_ref write_buf_;

  ///
  size_t bytes_wrote_ = 0;

  /// The error in async operation
  std::error_code ec_;
};

} // END namespace coro_async

#endif
