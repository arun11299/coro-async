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

#ifndef CORO_ASYNC_IO_SERVICE_HPP
#define CORO_ASYNC_IO_SERVICE_HPP

#include <queue>
#include <vector>
#include <functional>
#include "coro-async/detail/scheduler.hpp"
#include "coro-async/detail/epoll_reactor.hpp"

namespace coro_async {

/**
 * Talks to the scheduler and schedules tasks.
 * Runs the scheduler.
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
    scheduler_.schedule_after(secs, std::forward<T>(cb));
  }

  ///
  template <typename T>
  void schedule_after(std::chrono::milliseconds msecs, T&& cb)
  {
    scheduler_.schedule_after(msecs, std::forward<T>(cb));
  }

  ///
  void run();

  ///
  template <typename TaskFn>
  void post(TaskFn&& task);

private:
  /// Scheduler instance
  detail::scheduler scheduler_;
};

} // END namespace coro-async

#include "coro-async/impl/io_service.ipp"

#endif
