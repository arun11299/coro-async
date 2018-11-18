#ifndef CORO_ASYNC_SOCKET_OPS_IPP
#define CORO_ASYNC_SOCKET_OPS_IPP

#include "coro-async/error_codes.hpp"
#include "coro-async/detail/socket_ops.hpp"

extern "C" {
  #include <poll.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
}

namespace coro_async {
namespace detail {

void posix_socket_ops::bind(int sockfd, endpoint ep, std::error_code& ec)
{
  ec.clear();

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr = ep.address().native_addr();
  address.sin_port = ::htons(ep.port());

  int rc = ::bind(sockfd, (sockaddr*)&address, sizeof(sockaddr_in)); 
  if (rc != 0)
  {
    ec = std::error_code{errno, std::system_category()};
  }
  return;
}

void posix_socket_ops::listen(int sockfd, unsigned backlog, std::error_code& ec)
{
  ec.clear();

  int rc = ::listen(sockfd, backlog);
  if (rc != 0)
  {
    ec = std::error_code{errno, std::system_category()};
  }
  return;
}

void posix_socket_ops::connect(int sockfd, endpoint ep, std::error_code& ec)
{
  ec.clear();

  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr = ep.address().native_addr();
  address.sin_port = ::htons(ep.port());

  int rc = ::connect(sockfd, (sockaddr*)&address, sizeof(sockaddr_in));
  if (rc != 0)
  {
    switch (errno)
    {
      case EINPROGRESS:
        ec = error::socket_errc::in_progress;
        break;
      case ECONNREFUSED:
        ec = error::socket_errc::conn_refused;
        break;
      case EADDRINUSE:
        ec = error::socket_errc::addr_in_use;
        break;
      case EWOULDBLOCK:
        ec = error::socket_errc::would_block;
        break;
      default:
        ec = std::error_code{errno, std::system_category()};
        break;
    };
  }
  return;
}

std::pair<int, endpoint> posix_socket_ops::accept(int sockfd, std::error_code& ec)
{
  ec.clear();
  sockaddr_in client_addr;

  unsigned int sin_size = sizeof(struct sockaddr_in);
  int new_fd = ::accept(sockfd, (sockaddr*)&client_addr, &sin_size);
  if (new_fd == -1)
  {
    ec = std::error_code{errno, std::system_category()};
  }

  v4_address addr{client_addr.sin_addr};

  return {new_fd, endpoint{addr, client_addr.sin_port}};
}

bool posix_socket_ops::nb_connect(int sockfd, std::error_code& ec)
{
  ec.clear();

  pollfd fds;
  fds.fd = sockfd;
  fds.events = POLLOUT;
  fds.revents = 0;
  int ready = ::poll(&fds, 1, 0);
 
  if (!ready)
  {
    /// Still in progress
    return false;
  }
  // Check for connection errors that might have
  // happened while the operation was in progress
  int connect_error = 0;
  unsigned int connect_error_len = sizeof(connect_error);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
                   &connect_error, &connect_error_len) == 0)
  {
    if (connect_error)
    {
      ec = std::error_code{connect_error, std::system_category()};
    }
  }

  return true;
}

template <typename Buffer>
bool posix_socket_ops::nb_read(
    int sockfd, Buffer& buf, size_t& bytes_read, std::error_code& ec)
{
  ec.clear();

  while (true)
  {
    ssize_t rbytes = ::read(sockfd, buf.data(), buf.size());

    if (rbytes == 0)
    {
      ec = error::socket_errc::eof;
      return true;
    }

    if (rbytes >= 0)
    {
      bytes_read = rbytes;
      return true;
    }

    assert (rbytes == -1);

    // Should be here only for error cases
    switch (errno)
    {
      case EINTR:
        continue;
      case EAGAIN: // Same as EWOULDBLOCK
        ec = error::socket_errc::would_block;
        return false;
      case EBADF:
        ec = error::socket_errc::bad_file_descriptor;
        return false;
      case EINVAL:
        ec = error::socket_errc::invalid_descriptor;
        return false;
      case EFAULT:
        ec = error::socket_errc::bad_read_buffer;
        return false;
      case EIO:
        ec = error::socket_errc::io_error;
        return false;
      default:
        ec = error::socket_errc::unknown;
        return false;
    };
  }

  assert (0 && "Code not reached");
  return false;
}

template <typename Buffer>
bool posix_socket_ops::nb_write(
    int sockfd, const Buffer& buf, size_t& bytes_wrote, std::error_code& ec)
{
  ec.clear();

  while (true)
  {
    ssize_t wbytes = ::write(sockfd, buf.data(), buf.size());

    if (wbytes >= 0)
    {
      bytes_wrote = wbytes;
      return true;
    }

    switch (errno)
    {
      case EINTR:
        continue;
      case EAGAIN: // Same as EWOULDBLOCK
        ec = error::socket_errc::would_block;
        return false;
      case EBADF:
        ec = error::socket_errc::bad_file_descriptor;
        return false;
      case EINVAL:
        ec = error::socket_errc::invalid_descriptor;
        return false;
      case EFAULT:
        ec = error::socket_errc::bad_read_buffer;
        return false;
      case EIO:
        ec = error::socket_errc::io_error;
        return false;
      default:
        ec = error::socket_errc::unknown;
        return false;
    };
  }

  assert (0 && "Code not reached");
  return false;
}

} // END namspace detail
} // END namespace coro_async

#endif
