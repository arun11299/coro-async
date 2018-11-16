#ifndef CORO_ASYNC_coro_task_auto_HPP
#define CORO_ASYNC_coro_task_auto_HPP

#include <cassert>
#include <type_traits>
#include <functional>
#include <experimental/coroutine>

namespace stdex = std::experimental;

namespace coro_async {

/**
 */
namespace detail {

template <typename T>
struct callback
{
  std::function<void(T)> done_cb_ = nullptr;
};

template <>
struct callback<void>
{
  std::function<void()> done_cb_ = nullptr;
};

} // END anmespace detail

/**
 */
template <typename T=void>
struct promise_base : detail::callback<T>
{
  ///
  promise_base()
    : detail::callback<T>()
  {
  }

  promise_base(const promise_base&) = default;
  promise_base& operator=(const promise_base&) = default;

  /*!
   * Add done callback which would be executed
   * after coroutine is finished executing.
   */
  template <typename U>
  void add_done_callback(U&& handler)
  {
    this->done_cb_ = std::forward<U>(handler);
  }
};


/**
 */
template <typename T=void>
class coro_task_auto
{
public:
  /// The held type in promise
  using return_type = T;

  /// The promise type corresponding to the coro_task_auto<T>
  struct promise_type;

  ///
  coro_task_auto(promise_type* p)
    : coroutine_(stdex::coroutine_handle<promise_type>::from_promise(*p))
  {
  }

  coro_task_auto(coro_task_auto&& other)
    : coroutine_(other.coroutine_)
  {
    other.coroutine_ = nullptr;
  }

  coro_task_auto& operator=(coro_task_auto&& other)
  {
    coroutine_ = other.coroutine_;
    other.coroutine_ = nullptr;
  }

  ///
  coro_task_auto(const coro_task_auto&) = delete;
  coro_task_auto& operator=(const coro_task_auto&) = delete;

  ~coro_task_auto() = default;

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

  ///
  template <typename Handler>
  void add_done_callback(Handler&& h)
  {
    assert (coroutine_);
    coroutine_.promise().add_done_callback(std::forward<Handler>(h));
  }

  ///
  T result() noexcept
  {
    return std::move(coroutine_.promise().result());
  }

private:
  /// The coroutine handler
  stdex::coroutine_handle<promise_type> coroutine_ = nullptr;
};


/**
 * To be used when the task is to be used with RAII
 * cleanup.
 */
template <typename T=void>
class coro_task_manual
{
public:
  /// The held type in promise
  using return_type = T;

  /// The promise type corresponding to the coro_task_manual<T>
  struct promise_type;

  ///
  coro_task_manual(promise_type* p)
    : coroutine_(stdex::coroutine_handle<promise_type>::from_promise(*p))
  {
  }

  coro_task_manual(coro_task_manual&& other)
    : coroutine_(other.coroutine_)
  {
    other.coroutine_ = nullptr;
  }

  coro_task_manual& operator=(coro_task_manual&& other)
  {
    coroutine_ = other.coroutine_;
    other.coroutine_ = nullptr;
  }

  ///
  coro_task_manual(const coro_task_manual&) = delete;
  coro_task_manual& operator=(const coro_task_manual&) = delete;

  ~coro_task_manual()
  {
    if (coroutine_)
    {
      coroutine_.destroy();
      coroutine_ = nullptr;
    }
  }

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

  ///
  template <typename Handler>
  void add_done_callback(Handler&& h)
  {
    assert (coroutine_);
    coroutine_.promise().add_done_callback(std::forward<Handler>(h));
  }

  ///
  T result() noexcept
  {
    return std::move(coroutine_.promise().result());
  }

private:
  /// The coroutine handler
  stdex::coroutine_handle<promise_type> coroutine_ = nullptr;
};


/**
 */
template <typename T>
struct coro_task_auto<T>::promise_type : promise_base<T>
{
  ///
  promise_type()
    : promise_base<T>()
  {}

  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  // TODO: What about RAII based coroutine handles ?
  stdex::suspend_never final_suspend()
  {
    if (this->done_cb_) this->done_cb_(std::move(result()));
    return {};
  }

  ///
  // TODO: Need a variation which takes in the 
  // parameters passed to the coroutine function.
  coro_task_auto<T> get_return_object()
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


/**
 */
template <typename T>
struct coro_task_manual<T>::promise_type : promise_base<T>
{
  ///
  promise_type()
    : promise_base<T>()
  {
  }

  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  stdex::suspend_always final_suspend()
  {
    if (this->done_cb_) this->done_cb_(std::move(result()));
    return {};
  }

  ///
  // TODO: Need a variation which takes in the 
  // parameters passed to the coroutine function.
  coro_task_manual<T> get_return_object()
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
  T held_;
};

//=============================================================================
// void specialization
//=============================================================================

template <>
struct coro_task_auto<void>::promise_type : promise_base<void>
{
  ///
  promise_type()
    : promise_base<void>()
  {
  }

  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  // TODO: What about RAII based coroutine handles ?
  stdex::suspend_never final_suspend()
  {
    if (this->done_cb_) this->done_cb_();
    return {};
  }

  coro_task_auto<void> get_return_object()
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


/**
 */
template <>
struct coro_task_manual<void>::promise_type : promise_base<void>
{
  ///
  promise_type()
    : promise_base<void>()
  {
  }

  ///
  stdex::suspend_never initial_suspend()
  {
    return {};
  }

  ///
  stdex::suspend_always final_suspend()
  {
    if (this->done_cb_) this->done_cb_();
    return {};
  }

  coro_task_manual<void> get_return_object()
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
