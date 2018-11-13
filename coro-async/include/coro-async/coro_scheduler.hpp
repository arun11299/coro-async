#ifndef CORO_ASYNC_coro_scheduler_HPP
#define CORO_ASYNC_coro_scheduler_HPP

#include "coro-async/io_service.hpp"
#include "coro-async/coro/schedule_awaitable.hpp"

namespace coro_async {
/**
 */
class coro_scheduler
{
public:
  ///
  coro_scheduler(io_service& ios) 
    : ios_(ios)
  {
  }
  ///
  coro_scheduler(const coro_scheduler&) = delete;
  coro_scheduler& operator=(const coro_scheduler&) = delete;

public:
  ///
  timed_schedule_awaitable yield_for(std::chrono::seconds secs)
  {
    return { ios_, secs };
  }

  timed_schedule_awaitable yield_for(std::chrono::milliseconds msecs)
  {
    return { ios_, msecs };
  }

  template <typename Handler>
  task_completion_awaitable<Handler> wait_for(Handler&& h)
  {
    return { ios_, std::forward<Handler>(h) };
  }

private:
  /// The io_service reference
  io_service& ios_;
};

} // END namespace coro_async

#endif
