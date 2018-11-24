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

#ifndef CORO_ASYNC_DETAIL_TIMER_QUEUE_HPP
#define CORO_ASYNC_DETAIL_TIMER_QUEUE_HPP

#include <queue>
#include <chrono>
#include <vector>

namespace coro_async {
namespace detail     {

/**
 * A priority queue for storing timers.
 */
template <typename T>
class timer_queue
{
public:
  /// The type of duration stored
  using time_type = std::chrono::milliseconds;
  using element_type = std::pair<time_type, T>;
  using container_type = std::vector<element_type>;

public:
  ///
  timer_queue() = default;

  timer_queue(const timer_queue&) = default;
  timer_queue& operator=(const timer_queue&) = default;

public:
  ///
  void add(const std::chrono::seconds sec, T&& ctx)
  {
    const auto val = current_time() + time_type(sec);
    q_.push( {val, std::forward<T>(ctx)} );
  }

  ///
  void add(const std::chrono::milliseconds msec, T&& ctx)
  {
    const auto val = current_time() + msec;
    q_.push( {val, std::forward<T>(ctx)} );
  }

  ///
  const element_type& peek() const noexcept
  {
    return q_.top();
  }

  /// Removes the top element from the queue
  void remove()
  {
    q_.pop();
  }

  ///
  size_t size() const noexcept
  {
    return q_.size();
  }

  static time_type current_time()
  {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<time_type>(duration);
  }

private:
  struct element_type_cmp
  {
    bool operator()(const element_type& a, const element_type& b) const noexcept
    {
      return a.first > b.first;
    }
  };

  /// The timer queue
  std::priority_queue<element_type, container_type, element_type_cmp> q_;
};

} // END namespace detail
} // END namespace coro_async

#endif
