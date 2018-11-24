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
 * An awaitable interface for performing an operation after 
 * a duration.
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

  /// Schedules the resume operation after provided duration.
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

/**
 * An awaitable interface for completing the task.
 * The task itself could be a coroutine. In such cases,
 * it creates a stacked operation chain to finish
 * the operations one after the other.
 */
template <typename Handler>
class task_completion_awaitable: 
  public deduce_result_type<Handler>::type
{
public:
  /**
   * The `handler_return_type` and `actual_return_type` would be the
   * same in case of non coroutine tasks.
   */
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
    // If its a coroutine task, then add a done callback to
    // the coroutine, so that we can resume the current coroutine.
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
