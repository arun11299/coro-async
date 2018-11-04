#ifndef CORO_ASYNC_CORO_TASK_HPP
#define CORO_ASYNC_CORO_TASK_HPP

#include <cassert>
#include <type_traits>
#include <experimental/coroutine>

namespace stdex = std::experimental;

namespace coro_async {
/**
 */
template <typename T=void>
class coro_task
{
public:
  /// The promise type corresponding to the coro_task<T>
  struct promise_type;

  ///
  coro_task(promise_type* p)
    : coroutine_(stdex::coroutine_handle<promise_type>::from_promise(*p))
  {
  }

  coro_task(coro_task&& other)
    : coroutine_(other.coroutine_)
  {
    other.coroutine_ = nullptr;
  }

  coro_task& operator=(coro_task&& other)
  {
    coroutine_ = other.coroutine_;
    other.coroutine_ = nullptr;
  }

  ///
  coro_task(const coro_task&) = delete;
  coro_task& operator=(const coro_task&) = delete;

  ~coro_task() = default;

public:
  ///
  auto coroutine() noexcept
  {
    return coroutine_;
  }

  ///
  bool done() const noexcept
  {
    assert (coroutine_);
    return coroutine_.done();
  }

  ///
  void resume() noexcept
  {
    assert (coroutine_);
    coroutine_.resume();
  }

private:
  /// The coroutine handler
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
  // TODO: What about RAII based coroutine handles ?
  stdex::suspend_never final_suspend()
  {
    return {};
  }

  ///
  // TODO: Need a variation which takes in the 
  // parameters passed to the coroutine function.
  coro_task<T> get_return_object()
  {
    return { this };
  }

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U&&, T>>>
  void return_value(U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
  {
    new (&held_) T{std::forward<U>(value)};
  }

  ///
  T& result() noexcept
  {
    return held_;
  }

  ///
  void unhandled_exception()
  {
    // TODO: need to handle exception properly
    std::terminate();
  }

private:
  /// The returned object
  // TODO: Should be aligned storage ?
  T held_;
};


//=============================================================================
// void specialization
//=============================================================================

template <>
struct coro_task<void>::promise_type
{
  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  // TODO: What about RAII based coroutine handles ?
  stdex::suspend_never final_suspend()
  {
    return {};
  }

  coro_task<void> get_return_object()
  {
    return { this };
  }

  void return_void()
  {
    return;
  }

  void unhandled_exception()
  {
    // TODO: need to handle exception properly
    std::terminate();
  }
};

} // END namespace coro_async

#endif
