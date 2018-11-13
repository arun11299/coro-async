#ifndef CORO_ASYNC_IO_SERVICE_HPP
#define CORO_ASYNC_IO_SERVICE_HPP

#include <queue>
#include <vector>
#include <functional>
#include "coro-async/timer_queue.hpp"
#include "coro-async/detail/scheduler.hpp"
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
    return scheduler_.get_reactor();
  }

  ///
  template <typename T>
  void schedule_after(std::chrono::seconds secs, T&& cb)
  {
    timers_.add(secs, std::forward<T>(cb));
  }

  ///
  template <typename T>
  void schedule_after(std::chrono::milliseconds msecs, T&& cb)
  {
    timers_.add(msecs, std::forward<T>(cb));
  }

  ///
  void run();

  ///
  template <typename TaskFn>
  void post(TaskFn&& task);

private:
  /// Scheduler instance
  detail::scheduler scheduler_;
  ///
  timer_queue<std::function<void()>> timers_;
};

} // END namespace coro-async

#include "coro-async/impl/io_service.ipp"

#endif
