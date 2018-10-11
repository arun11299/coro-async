#ifndef CORO_ASYNC_IO_SERVICE_HPP
#define CORO_ASYNC_IO_SERVICE_HPP

#include "coro-async/detail/epoll_reactor.hpp"

namespace coro_async {

/**
 */
class io_service
{
public:
  /**
   */
  explicit io_service() = default;

  /// Non copyable and non assignable
  io_service(const io_service&) = delete;

  io_service(io_service&&) = delete;

  io_service& operator=(const io_service&) = delete;

  io_service& operator=(io_service&&) = delete;

public:
  ///
  detail::epoll_reactor& get_reactor() noexcept
  {
    return reactor_;
  }

  void run()
  {
    while ( true )
    {
      reactor_.run(0);
    }
  }

private:
  ///
  detail::epoll_reactor reactor_;
};

}

#endif
