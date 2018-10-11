#ifndef CORO_ASYNC_INT_ERROR_CODES_HPP
#define CORO_ASYNC_INT_ERROR_CODES_HPP

#include <system_error>

namespace coro_async {
namespace detail {

/**
 * Error codes for all the epoll
 * system call failures.
 */
enum class EpollErrc
{
  EpollCreateErr = 1,
};

/**
 */
std::error_code make_error_code(EpollErrc errc);

} // END namespace detail
} // END namespace coro-async

/// Make custom error codes
namespace std {

  template<>
  struct is_error_code_enum<coro_async::detail::EpollErrc> : true_type {};
}

#include "coro-async/detail/impl/internal_error_codes.ipp"

#endif
