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

#ifndef CORO_ASYNC_SCHEDULER_HPP
#define CORO_ASYNC_SCHEDULER_HPP

#include <mutex>
#include <system_error>
#include <condition_variable>

#include "coro-async/io_service.hpp"
#include "coro-async/detail/timer_queue.hpp"
#include "coro-async/detail/scheduler_op.hpp"
#include "coro-async/detail/epoll_reactor.hpp"
#include "coro-async/detail/operation_queue.hpp"

namespace coro_async {
namespace detail     {

/**
 * Schedules an operation.
 */
class scheduler
{
public:
  /// Default cons.
  scheduler();

  /// Non copyable and non assignable
  scheduler(const scheduler&) = delete;
  scheduler& operator=(const scheduler&) = delete;

public: // Scheduler APIs
  /// Get the underlying reactor.
  epoll_reactor& get_reactor() noexcept
  {
    return reactor_;
  }

  /**
   * Adds the operation to the queue for deferred
   * execution.
   */
  template <typename Handler>
  void post(scheduler_op<Handler>* op);

  /**
   * Schedules an operation after `secs` seconds.
   */
  template <typename T>
  void schedule_after(std::chrono::seconds secs, T&& cb);
 
  /// Schedules an operation after `msecs` milliseconds.
  template <typename T>
  void schedule_after(std::chrono::milliseconds msecs, T&& cb);

  /// Run the scheduler.
  void run(std::error_code& ec);

private:
  ///
  void do_run_locked(const std::error_code& ec);

private:
  /// The reactor
  epoll_reactor reactor_;

  /// The queue of operations that needs to be scheduled
  /// for execution.
  operation_queue<operation_base> op_q_;

  /// Timer queue
  timer_queue<std::function<void()>> timers_;

  /// Lock to protect access to operation_queue
  std::mutex op_q_lock_;

  /// Lock to protect timer queue access
  std::mutex timer_q_lock_;

  /// Wait event
  std::condition_variable wait_event_;
};

} // END namespace detsil
} // END namespace coro_async


#include "coro-async/detail/impl/scheduler.ipp"

#endif
