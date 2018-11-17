#ifndef CORO_ASYNC_SCHEDULE_AWAITABLE_HPP
#define CORO_ASYNC_SCHEDULE_AWAITABLE_HPP

#include <chrono>
#include <type_traits>
#include <experimental/coroutine>
#include "coro-async/io_service.hpp"
#include "coro-async/coro/result.hpp"
#include "coro-async/detail/meta.hpp"


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
    std::cout << "await_suspend timed\n";
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

/**
 */
template <typename Handler>
class task_completion_awaitable: 
  public deduce_result_type<Handler>::type
{
public:
  /// The return type of handler
  using handler_return_type = typename detail::meta::deduce_return_type<Handler>::handler_return_type;
  /// The actual return type
  using actual_return_type = typename detail::meta::deduce_return_type<Handler>::type;
  /// The result_type which wraps the actual_return_type
  using result_type = typename deduce_result_type<Handler>::type;

  ///
  task_completion_awaitable(io_service& ios, Handler&& h)
    : result_type()
    , ios_(ios)
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
    if constexpr (detail::meta::is_coro_task<handler_return_type>::value)
    {
      auto coro_handler = handler_();
      if constexpr (!std::is_void<actual_return_type>{})
      {
        coro_handler.add_done_callback(
              [this, ch](actual_return_type value)
              {
                this->ios_.post(
                      [this, ch, result{std::move(value)}]() mutable
                      {
                        result_type::construct(std::move(result));
                        ch.resume();
                      }
                    );
              }
            );
      }
      else
      {
        coro_handler.add_done_callback(
              [this, ch]()
              {
                this->ios_.post(
                      [ch]() mutable
                      {
                        ch.resume();
                      }
                    );
              }
            );
      }
    }
    else
    {
      ios_.post([this, ch]() { this->task_completed(ch); });
    }
  }

  ///
  result_type await_resume()
  {
    if constexpr (!std::is_void<actual_return_type>{})
    {
      return { std::move(result_type::result()) };
    }
    else
    {
      return result_type{};
    }
  }

private:
  ///
  template <typename PromiseType>
  void task_completed(stdex::coroutine_handle<PromiseType> ch)
  {
    static_assert(!detail::meta::is_coro_task<handler_return_type>::value, "Only to be called for non-coroutines");

    if constexpr (!std::is_void<actual_return_type>{})
    {
      result_type::construct(handler_());
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
