#ifndef CORO_ASYNC_RESULT_HPP
#define CORO_ASYNC_RESULT_HPP

#include <variant>
#include <system_error>

namespace coro_async {

/**
 */
template <typename T>
struct result_type
{
public:
  ///
  template <typename U>
  result_type(U&& res)
    : res_repr_(std::move(res))
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
  T& result() const noexcept
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
};


} // END namespace coro_async

#endif
