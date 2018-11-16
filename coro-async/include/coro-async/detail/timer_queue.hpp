#ifndef CORO_ASYNC_DETAIL_TIMER_QUEUE_HPP
#define CORO_ASYNC_DETAIL_TIMER_QUEUE_HPP

#include <queue>
#include <chrono>
#include <vector>

namespace coro_async {
namespace detail     {

/**
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
