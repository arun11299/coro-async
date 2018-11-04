#ifndef CORO_ASYNC_SCHEDULE_AWAITABLE_HPP
#define CORO_ASYNC_SCHEDULE_AWAITABLE_HPP

#include <chrono>
#include <experimental/coroutine>
#include "coro-async/io_service.hpp"

namespace stdex = std::experimental;

namespace coro_async {

/**
 */
class timed_schedule_awaitable
{
public:
  ///
  timed_schedule_awaitable(io_service& ios, std::chrono::milliseconds d)
    : ios_(ios)
    , duration_(d)
  {
  }

  ///
  timed_schedule_awaitable(io_service& ios, std::chrono::seconds d)
    : ios_(ios)
    , duration_(std::chrono::milliseconds(d))
  {
  }

  ///
  timed_schedule_awaitable(const timed_schedule_awaitable&) = delete;
  timed_schedule_awaitable& operator=(const timed_schedule_awaitable&) = delete;

public: // Awaitable interface
  ///
  bool await_ready()
  {
    return false;
  }

  ///
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    ios_.schedule_after(duration_, [ch]() mutable {
          ch.resume(); 
        });
  }

  ///
  void await_resume() noexcept
  {
    return;
  }

public:
  /// The io_service reference
  io_service& ios_;

  /// Duration / Time interval
  std::chrono::milliseconds duration_;
};

} // END namespace coro_async

#endif
