#ifndef CORO_ASYNC_INTERNAL_ERROR_CODES_IPP
#define CORO_ASYNC_INTERNAL_ERROR_CODES_IPP

#include <cassert>

namespace coro_async {
namespace detail {

struct EpollErrorCategory: std::error_category
{
  const char* name() const noexcept override
  {
    return "epoll";
  }

  /**
   * NOTE: ev is an errno
   */
  std::string message(int ev) const override
  {
    return std::strerror(ev);
  }
};


/// Create global objects for the error categories
const EpollErrorCategory theEpollErrorCategory {};

std::error_code make_error_code(EpollErrc err)
{
  return { static_cast<int>(err), theEpollErrorCategory };
}

} // END namespace detail
} // END namespace coro-async

#endif
