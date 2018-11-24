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

#ifndef CORO_ASYNC_RESULT_HPP
#define CORO_ASYNC_RESULT_HPP

#include <variant>
#include <system_error>
#include "coro-async/detail/meta.hpp"
#include "coro-async/coro/coro_task.hpp"

namespace coro_async {

/**
 * The result type for non coroutine based tasks.
 */
template <typename T>
struct result_type_non_coro
{
public:
  ///
  // TODO: Should allow convertible types too ?
  result_type_non_coro(
      T res,
      std::enable_if_t<!detail::meta::is_coro_task<T>{}, void>* _=nullptr)
    : res_repr_(std::move(res))
  {
  }

  ///
  result_type_non_coro(coro_task_auto<T>& t)
    : res_repr_(std::move(t.result()))
  {
  }

  ///
  result_type_non_coro(coro_task_manual<T>& t)
    : res_repr_(std::move(t.result()))
  {
  }

  ///
  result_type_non_coro(std::error_code ec)
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
 * The result type for non coroutine based tasks returning void.
 */
struct result_type_non_coro_void
{
  result_type_non_coro_void() = default;
};

/**
 * The result type for coroutine based task returning non-void
 * value.
 */
template <typename C>
struct result_type_coro_non_void
{
public:
  /// The unwrapped type
  using unwrapped_type = typename C::return_type;

  ///
  // TODO: maynot work for all `T`. Some `T`s wont
  // be default constructible.
  result_type_coro_non_void() = default;

  result_type_coro_non_void(unwrapped_type res)
    : res_repr_(std::move(res))
  {
  }

  result_type_coro_non_void(std::error_code ec)
    : res_repr_(ec)
  {
  }

  ///
  void construct(unwrapped_type&& res)
  {
    res_repr_ = std::forward<unwrapped_type>(res);
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
  unwrapped_type& result() noexcept
  {
    return std::get<unwrapped_type>(res_repr_);
  }

private:
  /// Internal representation of result type
  std::variant<unwrapped_type, std::error_code> res_repr_;
};


/**
 * The result type for coroutine task returning
 * void.
 */
struct result_type_coro_void
{
  using unwrapped_type = void;
};


/**
 * Get the result type to use.
 */
template <typename Handler>
auto get_result_type_type()
  -> std::enable_if_t<
       /// Check if the return type of the handler is coroutine
       detail::meta::is_coro_task<
         typename detail::meta::deduce_return_type<Handler>::handler_return_type>{},
       /// The handler returns a coroutine, now check if it returns a void
       /// or non-void result.
       std::conditional_t<
         std::is_void<typename detail::meta::deduce_return_type<Handler>::type>{},
         result_type_coro_void,
         result_type_coro_non_void<
            typename detail::meta::deduce_return_type<Handler>::handler_return_type
          >
       >
     >;

template <typename Handler>
auto get_result_type_type()
  -> std::enable_if_t<
       /// Check if the return type of the handler is not a coroutine
       !detail::meta::is_coro_task<
         typename detail::meta::deduce_return_type<Handler>::handler_return_type>{},
       /// The handler does not return a coroutine, check if it returns a void
       /// or non-void result.
       std::conditional_t<
         std::is_void<typename detail::meta::deduce_return_type<Handler>::type>{},
         result_type_non_coro_void,
         result_type_non_coro<
            typename detail::meta::deduce_return_type<Handler>::type
          >
       >
     >;
 

/**
 * TODO: Should go to meta
 */
template <typename Handler>
struct deduce_result_type
{
  using type = decltype(get_result_type_type<Handler>());
};

} // END namespace coro_async

#endif
