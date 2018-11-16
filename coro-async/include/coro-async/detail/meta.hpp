#ifndef CORO_ASYNC_META_HPP
#define CORO_ASYNC_META_HPP

#include <type_traits>
#include "coro-async/coro/coro_task.hpp"

namespace coro_async {
namespace detail     {
namespace meta       {

/// The void_t trick
template <typename...>
using void_t = void;

/**
 */
template <typename T>
struct is_coro_task : std::false_type
{
};

template <typename T>
struct is_coro_task<coro_task_auto<T>> : std::true_type
{
};

template <typename T>
struct is_coro_task<coro_task_manual<T>> : std::true_type
{
};

/**
 * Deduce handler return type.
 * If coro_task, return the underlying promise return_type.
 */
template <typename Handler>
struct deduce_return_type
{
  using handler_return_type = decltype(std::declval<Handler>()());

  using type = typename std::conditional<
      is_coro_task<handler_return_type>{},
      typename handler_return_type::return_type,
      handler_return_type
    >::type;
};

} // END namespace meta
} // END namesapce detail
} // END namespace coro_async

#endif
