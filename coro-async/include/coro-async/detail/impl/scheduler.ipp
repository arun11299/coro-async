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

void scheduler::run(std::error_code& ec)
{
  while (true)
  {
    std::lock_guard<std::mutex> guard{op_q_lock_};
    do_run_locked(ec);
  }
}

void scheduler::do_run_locked(const std::error_code& ec)
{
  // Run the reactor
  reactor_.run(2);

  while (!op_q_.is_empty())
  {
    auto op = op_q_.pop();

    // safe to remove the lock (TODO: ???)
    op_q_lock_.unlock();

    // do the call to handler
    op->call(op, ec, 0);
  }

  // Nothing to do...
  // std::unique_lock<std::mutex> lk{op_q_lock_};
  // wait_event_.wait(lk, [this] { return !this->op_q_.is_empty(); });
}

} // END namespace detail
} // END namespace coro_async

#endif
