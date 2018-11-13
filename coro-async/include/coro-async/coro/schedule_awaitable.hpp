#ifndef CORO_ASYNC_SCHEDULE_AWAITABLE_HPP
#define CORO_ASYNC_SCHEDULE_AWAITABLE_HPP

#include <chrono>
#include <type_traits>
#include <experimental/coroutine>
#include "coro-async/io_service.hpp"
#include "coro-async/coro/result.hpp"

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

//================================================================================

template <typename Handler>
using handler_return_type = typename std::decay<decltype(std::declval<Handler>()())>::type;

/**
 */
template <typename Handler>
class task_completion_awaitable: public result_type<handler_return_type<Handler>>
{
public:
  // The return type of handler
  using return_type = handler_return_type<Handler>;

  ///
  task_completion_awaitable(io_service& ios, Handler&& h)
    : ios_(ios)
    , handler_(std::forward<Handler>(h))
  {
  }

  ///
  task_completion_awaitable(const task_completion_awaitable&) = delete;
  task_completion_awaitable& operator=(const task_completion_awaitable&) = delete;

public: // Awaitable interface

  ///
  bool await_ready()
  {
    // TODO: optimization to see if we can complete inline ?
    return false;
  }

  ///
  template <typename PromiseType>
  void await_suspend(stdex::coroutine_handle<PromiseType> ch)
  {
    ios_.post([this, ch]() {
          this->task_completed(ch);
        });
  }

  ///
  result_type<return_type> await_resume()
  {
    if constexpr (!std::is_void<return_type>::value)
    {
      return { result_type<return_type>::result() };
    }
    else
    {
      return result_type<void>{};
    }
  }

private:
  ///
  template <typename PromiseType>
  void task_completed(stdex::coroutine_handle<PromiseType> ch)
  {
    if constexpr (!std::is_void<return_type>::value)
    {
      auto res = handler_();
      result_type<return_type>::construct(std::move(res));
    }
    else
    {
      handler_();
    }
    ch.resume();
  }

public:
  /// The io_service reference
  io_service& ios_;

  /// Final handler
  Handler handler_;
};

} // END namespace coro_async

#endif
