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

#ifndef CORO_ASYNC_READ_AWAITABLE_HPP
#define CORO_ASYNC_READ_AWAITABLE_HPP

#include "coro-async/coro/result.hpp"
#include "coro-async/detail/meta.hpp"

namespace stdex = std::experimental;

namespace coro_async {

/**
 * An awaitable class for performing socket read operation.
 */
class read_awaitable
{
public:
  /**
   * Constructor.
   * \param sock - The socket on which read is to be performed.
   * \param read_bytes - Number of bytes to read.
   * \param buf - The buffer into which the data needs to be wrote into.
   */
  read_awaitable(stream_socket& sock, size_t read_bytes, buffer::buffer_ref buf)
    : sock_(sock)
    , bytes_to_read_(read_bytes)
    , read_buf_(buf)
  {
    ec_.clear();
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

  /// Starts the asynchronous read operation.
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

  /**
   * Returns the result of the async read operation
   * wrapped inside `result_type_non_coro` type.
   * Wrapped result is number of bytes read.
   * In case of error, the wrapped value is the error_code.
   */
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
