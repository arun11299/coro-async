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

#ifndef CORO_ASYNC_coro_scheduler_HPP
#define CORO_ASYNC_coro_scheduler_HPP

#include "coro-async/io_service.hpp"
#include "coro-async/coro/schedule_awaitable.hpp"

namespace coro_async {

/**
 * Schedules tasks.
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
