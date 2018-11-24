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

#ifndef CORO_ASYNC_SOCKET_OPS_HPP
#define CORO_ASYNC_SOCKET_OPS_HPP

#include "coro-async/buffers.hpp"
#include "coro-async/endpoint.hpp"
#include "coro-async/detail/descriptor.hpp"

namespace coro_async {
namespace detail {

/**
 * A wrapper for POSIX socket system calls.
 */
class posix_socket_ops
{
public:
  /// `bind` system call
  static void bind(int sockfd, endpoint ep, std::error_code& ec);

  /// `listen` system call
  static void listen(int sockfd, unsigned backlog, std::error_code& ec);

  /// `connect` system call
  static void connect(int sockfd, endpoint ep, std::error_code& ec);

  /**
   * `accept` system call
   * Returns a pair consisting of the new socket and the peer endpoint
   */
  static std::pair<int, endpoint> accept(int sockfd, std::error_code& ec);

  /// `connect` system call (always on non-blocking socket)
  static bool nb_connect(int sockfd, std::error_code& ec);

  /**
   * Non blocking socket read.
   *
   * Returns-
   * false : When further reading resulted in WOULDBLOCK.
   * true : All bytes read.
   */
  template <typename Buffer>
  static bool nb_read(int sockfd, Buffer& buf, size_t& bytes_read, std::error_code& ec);

  /**
   * Non blocking socket write.
   *
   * Returns-
   * true: All the data got transferred.
   * false: All data was not transferred.
   */
  template <typename Buffer>
  static bool nb_write(
      int sockfd, const Buffer& buf, size_t& bytes_wrote, std::error_code& ec);
};

} // END namespace detail
} // END namespace coro_async

#include "coro-async/detail/impl/socket_ops.ipp"

#endif
