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

#ifndef CORO_ASYNC_SOCKET_HPP
#define CORO_ASYNC_SOCKET_HPP

#include <string>

#include "coro-async/endpoint.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/ip_address.hpp"
#include "coro-async/buffer_ref.hpp"
#include "coro-async/detail/descriptor.hpp"
#include "coro-async/detail/socket_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {

/**
 * A streaming socket handle.
 * Has APIs exposinf the async operations
 * that can be performed on the socket.
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
    , ios_(io_srv)
  {
  }

  /// Move copy constructible
  stream_socket(stream_socket&& other)
    : reactor_(other.reactor_)
    , ios_(other.ios_)
  {
    impl_.desc_ = std::move(other.impl_.desc_);
    impl_.desc_state_ = other.impl_.desc_state_;
    other.impl_.desc_state_ = nullptr;
  }

  stream_socket(const stream_socket&) = delete;

  stream_socket& operator=(const stream_socket&) = delete;

  ~stream_socket()
  {
    if (impl_.desc_state_)
    {
      delete impl_.desc_state_;
      impl_.desc_state_ = nullptr;
    }
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

  /// Close the socket
  void close()
  {
    assert (impl_.desc_state_);
    delete impl_.desc_state_;
    impl_.desc_state_ = nullptr;
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

  /**
   */
  template <typename CompletionHandler>
  void start_connect_op(endpoint ep, CompletionHandler&& ch);

public: // Async Operations
  /**
   */
  template <typename CompletionHandler>
  void async_connect(endpoint ep, CompletionHandler&& ch);

  /**
   * Reads atmost buf.size() data into the Buffer.
   * Buffer must exist till async_read_some finishes execution.
   */
  // NOTE: basic implemenatation. No iovec and buffer
  // concept for now.
  template <typename Buffer, typename ReadHandler>
  void async_read_some(const Buffer& buf, ReadHandler&& rh);

  /**
   * Makes sure that it reads atleast `buf.size()` data.
   * In case more data is read, will resize the buffer.
   */
  template <typename ReadHandler>
  void async_read(buffer::buffer_ref& buf, ReadHandler&& rh);

  /**
   * Writes atmost buf.size() data to the socket.
   * Buffer must exist till async_write_some finishes execution.
   */
  // NOTE: basic implementation. No iovec and buffer
  // concept for now.
  template <typename Buffer, typename WriteHandler>
  void async_write_some(const Buffer& buf, WriteHandler&& wh);

  /**
   * Makes sure that it writes atleast all the data in the
   * buffer.
   */
  template <typename WriteHandler>
  void async_write(buffer::buffer_ref& buf, WriteHandler&& wh);

private:
  ///
  implementation impl_;
  ///
  detail::epoll_reactor& reactor_;
  ///
  io_service& ios_;
};


} // END namespace coro-async

#include "coro-async/impl/stream_socket.ipp"


#endif
