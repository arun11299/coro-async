#ifndef CORO_ASYNC_SOCKET_HPP
#define CORO_ASYNC_SOCKET_HPP

#include <string>

#include "coro-async/buffers.hpp"
#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/ip_address.hpp"
#include "coro-async/detail/descriptor.hpp"
#include "coro-async/detail/socket_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {

/**
 */
class stream_socket
{
private:
  ///
  struct implementation
  {
    detail::descriptor desc_;
    //Allocated when called register_descriptor
    detail::descriptor_state* desc_state_ = nullptr;
  };

public:
  /**
   */
  stream_socket(io_service& io_srv)
    : reactor_(io_srv.get_reactor())
  {
  }

public:
  /// Check if socket is created successfully
  bool is_open() const noexcept
  {
    return impl_.desc_.get() != -1;
  }

  /// Opens up the socket and registers with reactor
  bool open(std::error_code& ec)
  {
    ec.clear();

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
      ec = std::error_code{errno, std::system_category()};
      return false;
    }
 
    impl_.desc_.set(sd);

    int rc = reactor_.register_descriptor(impl_.desc_, &impl_.desc_state_);
    if (rc != 0)
    {
      ec = std::error_code{errno, std::system_category()};
      return false;
    }

    return true;
  }

  /// Assign a descriptor
  bool assign(int fd, std::error_code& ec)
  {
    ec.clear();

    assert (!is_open());

    impl_.desc_.set(fd);
    std::cout << "Socket opened: " << fd << std::endl;

    int rc = reactor_.register_descriptor(impl_.desc_, &impl_.desc_state_);
    if (rc != 0)
    {
      ec = std::error_code{errno, std::system_category()};
      return false;
    }
    return true;
  }

  /// Get the native socket descriptor
  typename detail::descriptor::descriptor_type
  get_native_handle() const noexcept
  {
    return impl_.desc_.get();
  }

  /// Binds the socket with (ip:port)
  void bind(endpoint ep, std::error_code& ec)
  {
    ec.clear();

    if (!is_open())
    {
      if (!open(ec)) return;
    }
    detail::posix_socket_ops::bind(get_native_handle(), ep, ec);
    return;
  }

  /**
   */
  void listen(unsigned backlog, std::error_code& ec)
  {
    ec.clear();

    if (!is_open())
    {
      if (!open(ec)) return;
    }
    detail::posix_socket_ops::listen(get_native_handle(), backlog, ec);
    return;
  }

  /**
   */
  void start_reactor_op(enum reactor_ops r_op, detail::operation_base* op)
  {
    reactor_.start_op(impl_.desc_, impl_.desc_state_, r_op, op);
  }

public: // Async Operations
  /**
   */
  template <typename CompletionHandler>
  void async_connect(endpoint ep, CompletionHandler&& ch);

  /**
   * Reads atmost buf.size() data into the buffer.
   * Buffer must exist till async_read_some finishes execution.
   */
  // NOTE: basic implemenatation. No iovec and buffer
  // concept for now.
  template <typename ReadHandler>
  void async_read_some(buffer::Buffer& buf, ReadHandler&& rh);

  /**
   * Writes atmost buf.size() data to the socket.
   * Buffer must exist till async_write_some finishes execution.
   */
  // NOTE: basic implementation. No iovec and buffer
  // concept for now.
  template <typename WriteHandler>
  void async_write_some(const buffer::Buffer& buf, WriteHandler&& wh);

private:
  ///
  implementation impl_;
  ///
  detail::epoll_reactor& reactor_;
};


} // END namespace coro-async

#include "coro-async/impl/stream_socket.ipp"


#endif
