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
 */
class scheduler
{
public:
  ///
  scheduler();

  /// Non copyable and non assignable
  scheduler(const scheduler&) = delete;
  scheduler& operator=(const scheduler&) = delete;

public: // Scheduler APIs
  ///
  epoll_reactor& get_reactor() noexcept
  {
    return reactor_;
  }

  ///
  template <typename Handler>
  void post(scheduler_op<Handler>* op);

  ///
  template <typename T>
  void schedule_after(std::chrono::seconds secs, T&& cb);
 
  ///
  template <typename T>
  void schedule_after(std::chrono::milliseconds msecs, T&& cb);

  ///
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
