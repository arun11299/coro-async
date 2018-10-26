#ifndef CORO_ASYNC_CORO_TASK_HPP
#define CORO_ASYNC_CORO_TASK_HPP

#include <experimental/coroutine>

namespace stdex = std::experimental;

namespace coro_async {
/**
 */
template <typename T>
struct coro_task
{
  struct promise_type;

  ///
  coro_task() = default;

  ///
  explicit coro_task(promise_type* p)
    : coroutine_(stdex::coroutine_handle<promise_type>::from_promise(*p))
  {
  }

  ///
  coro_task(const coro_task&) = delete;
  coro_task& operator=(const coro_task&) = delete;

  ///
  coro_task(coro_task&& other)
    : held_(std::move(other.held_))
    , coroutine_(other.coroutine_)
  {
    other.coroutine_ = nullptr;
  }

  ///
  coro_task& operator=(coro_task&& other)
  {
    held_ = std::move(other.held_);
    coroutine_ = other.coroutine_;
    other.coroutine_ = nullptr;
  }

  ///
  ~coro_task()
  {
  }

private:
  ///
  T held_;
  /// 
  stdex::coroutine_handle<promise_type> coroutine_ = nullptr;
};


/**
 */
template <typename T>
struct coro_task<T>::promise_type
{
  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  stdex::suspend_always final_suspend()
  {
    return {};
  }

  ///
  coro_task<T> get_return_object()
  {
    return coro_task<T>{this};
  }

  void return_void() {}
  void unhandled_exception() { std::terminate(); }
};

} // END namespace coro_async

#endif
