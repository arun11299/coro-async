#ifndef CORO_ASYNC_SCHEDULER_IPP
#define CORO_ASYNC_SCHEDULER_IPP

#include <cassert>

namespace coro_async {
namespace detail {

scheduler::scheduler()
{
}

template <typename Handler>
void scheduler::post(scheduler_op<Handler>* op)
{
  std::lock_guard<std::mutex> guard{op_q_lock_};
  op_q_.push(op);
}

template <typename T>
void scheduler::schedule_after(std::chrono::seconds secs, T&& cb)
{
  std::lock_guard<std::mutex> guard{timer_q_lock_};
  timers_.add(secs, std::forward<T>(cb));
}

template <typename T>
void scheduler::schedule_after(std::chrono::milliseconds msecs, T&& cb)
{
  timers_.add(msecs, std::forward<T>(cb));
}

void scheduler::run(std::error_code& ec)
{
  while (true)
  {
    //std::lock_guard<std::mutex> guard{op_q_lock_};
    do_run_locked(ec);
  }
}

void scheduler::do_run_locked(const std::error_code& ec)
{
  // Run the reactor
  reactor_.run(2);

  while (!op_q_.is_empty())
  {
    op_q_lock_.lock();
    auto op = op_q_.pop();
    op_q_lock_.unlock();

    // do the call to handler
    op->call(op, ec, 0);
  }
  
  {
    std::lock_guard<std::mutex> guard{timer_q_lock_};
    auto curr_time = timers_.current_time();

    while (timers_.size())
    {
      // TODO: Copy ?
      auto element = timers_.peek();
      if (curr_time >= element.first)
      {
        (element.second)();
        timers_.remove();
      }
      else
      {
        break;
      }
    }
  } // timer scope end

  // Nothing to do...
  // std::unique_lock<std::mutex> lk{op_q_lock_};
  // wait_event_.wait(lk, [this] { return !this->op_q_.is_empty(); });
}

} // END namespace detail
} // END namespace coro_async

#endif
