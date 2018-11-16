#ifndef CORO_ASYNC_RESULT_HPP
#define CORO_ASYNC_RESULT_HPP

#include <variant>
#include <system_error>
#include "coro-async/detail/meta.hpp"
#include "coro-async/coro/coro_task.hpp"

namespace coro_async {

/**
 */
template <typename T, typename=void>
struct result_type
{
public:
  ///
  template <typename U, 
            typename=typename std::enable_if_t<
              !detail::meta::is_coro_task<U>{}>
           >
  result_type(U&& res)
    : res_repr_(std::move(res))
  {
  }

  template <typename U>
  result_type(coro_task_auto<U>& t)
    : res_repr_(std::move(t.result()))
  {
  }

  template <typename U>
  result_type(coro_task_manual<U>& t)
    : res_repr_(std::move(t.result()))
  {
  }

  result_type(std::error_code ec)
    : res_repr_(ec)
  {
  }

  ///
  template <typename... Args>
  void construct(Args&&... args)
  {
    res_repr_ = T{std::forward<Args>(args)...};
  }

  ///
  bool is_error() const noexcept
  {
    return std::holds_alternative<std::error_code>(res_repr_);
  }

  ///
  std::error_code error() const noexcept
  {
    return std::get<std::error_code>(res_repr_);
  }

  ///
  T& result() noexcept
  {
    return std::get<T>(res_repr_);
  }

private:
  /// Internal representation of result type
  std::variant<T, std::error_code> res_repr_;
};

/**
 */
template <>
struct result_type<void>
{
  result_type() = default;
};

/**
 * Specialization for coro_task to unwrap the result
 */
template <typename C>
struct result_type<C, detail::meta::void_t<
                        typename std::enable_if_t<
                          detail::meta::is_coro_task<C>::value &&
                          !std::is_void<typename C::return_type>::value>
                      >
                  >
{
public:
  /// The unwrapped type
  using unwrapped_type = typename C::return_type;

  ///
  // TODO: maynot work for all `T`. Some `T`s wont
  // be default constructible.
  result_type() = default;

  result_type(std::error_code ec)
    : res_repr_(ec)
  {
  }

  ///
  void construct(C&& arg)
  {
    res_repr_ = arg.result();
  }

  ///
  bool is_error() const noexcept
  {
    return std::holds_alternative<std::error_code>(res_repr_);
  }
 
  ///
  std::error_code error() const noexcept
  {
    return std::get<std::error_code>(res_repr_);
  }

  ///
  unwrapped_type& result() const noexcept
  {
    return std::get<unwrapped_type>(res_repr_);
  }

private:
  /// Internal representation of result type
  std::variant<unwrapped_type, std::error_code> res_repr_;
};


/**
 */
template <typename C>
struct result_type<C, 
                   typename std::enable_if_t<
                     detail::meta::is_coro_task<C>::value,
                     void>
                  >
{
  using unwrapped_type = void;
};

} // END namespace coro_async

#endif
