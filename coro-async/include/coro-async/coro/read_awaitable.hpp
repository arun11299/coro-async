#ifndef CORO_ASYNC_READ_AWAITABLE_HPP
#define CORO_ASYNC_READ_AWAITABLE_HPP

#include <experimental/coroutine>
#include "coro-async/buffer_ref.hpp"
#include "coro-async/stream_socket.hpp"

namespace stdex = std::experimental;

namespace coro_async {

/**
 */
class read_awaitable
{
public:
  /// 
  read_awaitable(stream_socket& sock, size_t read_bytes, buffer::buffer_ref& buf)
    : sock_(sock)
    , bytes_to_read_(read_bytes)
    , read_buf_(read_buf_)
  {
  }

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
  void await_resume()
  {
    //TODO: pass read data or size ?
    return;
  }

private: // hidden implementation
  ///
  template <typename PromiseType>
  void handle_read_cb(
      const stdex::coroutine_handle<PromiseType> ch,
      const std::error_code ec,
      const size_t rd_bytes)
  {
#include <iostream>
    std::cout << "Coroutine read callback worked!!!" << std::endl;
  }

private:
  /// The underlying streaming socket reference
  stream_socket& sock_;
  /// Exact bytes to be read
  size_t bytes_to_read_ = 0;
  /// The buffer reference into which data neads to be read
  buffer::buffer_ref& read_buf_;
  
};

} // END namespace coro_async


#endif
