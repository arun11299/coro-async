#ifndef CORO_ASYNC_READ_AWAITABLE_HPP
#define CORO_ASYNC_READ_AWAITABLE_HPP

#include "coro-async/coro/result.hpp"
#include "coro-async/detail/meta.hpp"

namespace stdex = std::experimental;

namespace coro_async {

/**
 */
class read_awaitable
{
public:
  /// 
  read_awaitable(stream_socket& sock, size_t read_bytes, buffer::buffer_ref buf)
    : sock_(sock)
    , bytes_to_read_(read_bytes)
    , read_buf_(buf)
  {
    ec_.clear();
  }

  ///

  ///
  read_awaitable(const read_awaitable&) = delete;
  ///
  read_awaitable& operator=(const read_awaitable&) = delete;
  ///
  ~read_awaitable() = default;

public: // Awaitable implementation
  ///
  bool await_ready()
  {
    //TODO: socket read optimizations
    return false;
  }

  ///
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    sock_.async_read(
        read_buf_, [this, ch](const std::error_code ec, const size_t rd_bytes) {
                     this->handle_read_cb(ch, ec, rd_bytes);
                     return;
                   }
       );
  }

  ///
  result_type_non_coro<size_t> await_resume()
  {
    if (ec_) return { ec_ };
    else     return { bytes_read_ };
  }

private: // hidden implementation
  ///
  template <typename PromiseType>
  void handle_read_cb(stdex::coroutine_handle<PromiseType> ch,
                      const std::error_code ec,
                      const size_t rd_bytes)
  {
    if (ec) ec_ = ec;
    else bytes_read_ = rd_bytes;

    ch.resume();
  }

private:
  /// The underlying streaming socket reference
  stream_socket& sock_;

  /// Exact bytes to be read
  const size_t bytes_to_read_ = 0;

  /// Bytes read from the network
  size_t bytes_read_ = 0;

  /// The buffer reference into which data neads to be read
  buffer::buffer_ref read_buf_;

  /// The error in async operation
  std::error_code ec_;
};

} // END namespace coro_async


#endif
