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

#ifndef CORO_ASYNC_CORO_SOCKET_HPP
#define CORO_ASYNC_CORO_SOCKET_HPP

#include "coro-async/buffer_ref.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/coro/read_awaitable.hpp"
#include "coro-async/coro/write_awaitable.hpp"

namespace coro_async {

/**
 * A thin wrapper over `stream_socket` for providing
 * an awaitable based interface for read and write.
 */
class coro_socket
{
public:
  ///
  coro_socket(io_service& ios)
    : ios_(ios)
    , sock_(ios)
  {
  }

  coro_socket(coro_socket&& other) = default;

  ~coro_socket() = default;

public:
  ///
  stream_socket& get_stream_sock() noexcept
  {
    return sock_;
  }

  ///
  io_service& get_io_service() noexcept
  {
    return ios_;
  }

  ///
  void close()
  {
    sock_.close();  
  }

public: // The awaitables
  ///
  //TODO: This buffer_ref thing sucks beyond my imagination
  auto read(size_t bytes, buffer::buffer_ref& buf)
  {
    return read_awaitable{sock_, bytes, buf};
  }

  ///
  auto write(size_t bytes, buffer::buffer_ref& buf)
  {
    return write_awaitable{sock_, bytes, buf};
  }

private:
  /// The io_service
  io_service& ios_;

  /// The underlying stream socket
  stream_socket sock_;
};

} // END namespace coro_async

#endif
