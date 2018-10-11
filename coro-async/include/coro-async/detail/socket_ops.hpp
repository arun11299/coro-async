#ifndef CORO_ASYNC_SOCKET_OPS_HPP
#define CORO_ASYNC_SOCKET_OPS_HPP

#include "coro-async/buffers.hpp"
#include "coro-async/endpoint.hpp"
#include "coro-async/detail/descriptor.hpp"

namespace coro_async {
namespace detail {

/**
 */
class posix_socket_ops
{
public:
  /// `bind` system call
  static void bind(int sockfd, endpoint ep, std::error_code& ec);

  /// `listen` system call
  static void listen(int sockfd, unsigned backlog, std::error_code& ec);

  /**
   * `accept` system call
   * Returns a pair consisting of the new socket and the peer endpoint
   */
  static std::pair<int, endpoint> accept(int sockfd, std::error_code& ec);

  /**
   * Non blocking socket read.
   *
   * Returns-
   * false : When further reading resulted in WOULDBLOCK.
   * true : All bytes read.
   */
  static bool nb_read(int sockfd, buffer::Buffer& buf, size_t& bytes_read, std::error_code& ec);

  /**
   * Non blocking socket write.
   *
   * Returns-
   * true: All the data got transferred.
   * false: All data was not transferred.
   */
  static bool nb_write(
      int sockfd, const buffer::Buffer& buf, size_t& bytes_wrote, std::error_code& ec);
};

} // END namespace detail
} // END namespace coro_async

#include "coro-async/detail/impl/socket_ops.ipp"

#endif
