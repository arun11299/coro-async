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

#ifndef CORO_ASYNC_EPOLL_HPP
#define CORO_ASYNC_EPOLL_HPP

#include <sys/epoll.h>
#include <cstring>
#include <cassert>
#include <exception>
#include <system_error>

extern "C" {
#include <unistd.h>
#include <fcntl.h>
}

#include "coro-async/detail/descriptor.hpp"
#include "coro-async/detail/internal_error_codes.hpp"

namespace coro_async {
namespace detail {

/**
 * Linux epoll wrapper.
 * Move only.
 */
class Epoll
{
public: // 'tors
  /**
   * Creates an epoll file descriptor.
   *
   * Exception:
   *  Throws `std::system_error` on epoll creation failure.
   */
  Epoll()
  {
    // close on exec is default choice here
    // TODO: might want to make in configurable.
    epfd_ = epoll_create1(EPOLL_CLOEXEC);

    if (epfd_ == -1) {
      std::error_code ec = std::error_code{errno, theEpollErrorCategory};
      throw std::system_error{ec, std::strerror(errno)};
    }
  }

  ~Epoll()
  {
    // we cannot be in this state
    assert (epfd_ != -1);
    // TODO: Report errors closing the file
    close(epfd_);
    epfd_ = -1;
  }

  /// Non copyable, assignable and movable
  
  Epoll(const Epoll&) = delete;

  Epoll& operator=(const Epoll&) = delete;

public: // Exposed APIs
  /**
   */
  int get() const noexcept
  {
    // we cannot be in this state
    assert (epfd_ != -1);
    return epfd_;
  }

  /**
   * NOTE: Upto the higher layers to do duplicate check or not.
   */
  void add_descriptor(descriptor::descriptor_type fd, epoll_event* event, std::error_code& ec)
  {
    ec.clear();
    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, event);
    if (ret == -1)
    {
      ec = std::error_code{errno, theEpollErrorCategory};
    }
    return;
  }

  /**
   * NOTE: Upto the higher layers to do duplicate check or not.
   */
  void modify_descriptor(descriptor::descriptor_type fd, epoll_event* event, std::error_code& ec)
  {
    ec.clear();
    int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, event);
    if (ret == -1)
    {
      ec = std::error_code{errno, theEpollErrorCategory};
    }
    return;
  }

  /**
   * NOTE: Upto the higher layers to do duplicate check or not.
   */
  void delete_descriptor(descriptor::descriptor_type fd, epoll_event* event, std::error_code& ec)
  {
    ec.clear();
    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, event);
    if (ret == -1)
    {
      ec = std::error_code{errno, theEpollErrorCategory};
    }
    return;
  }

private:
  /// The spoll file descriptor
  int epfd_ = -1;
};
 
} // END namesapce detail
} // END namespace coro-async

#endif
