#ifndef CORO_ASYNC_SCHEDULER_HPP
#define CORO_ASYNC_SCHEDULER_HPP

#include "coro-async/io_service.hpp"
#include "coro-async/coro/schedule_awaitable.hpp"

namespace coro_async {
/**
 */
class scheduler
{
public:
  ///
  scheduler(io_service& ios) 
    : ios_(ios)
  {
  }
  ///
  scheduler(const scheduler&) = delete;
  scheduler& operator=(const scheduler&) = delete;

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

private:
  /// The io_service reference
  io_service& ios_;
};

} // END namespace coro_async

#endif
