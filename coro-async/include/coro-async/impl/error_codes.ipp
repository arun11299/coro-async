#ifndef CORO_ASYNC_ERROR_CODES_IPP
#define CORO_ASYNC_ERROR_CODES_IPP

#include <cassert>

namespace coro_async {
namespace error {

// Anonymous namespace
namespace {

/**
 */
struct socket_err_category: std::error_category
{
  const char* name() const noexcept override
  {
    return "socket";
  }

  std::string message(int ev) const override
  {
    switch (static_cast<socket_errc>(ev))
    {
      case socket_errc::eof:
        return "eof";
      case socket_errc::try_again:
        return "try again";
      case socket_errc::would_block:
        return "would block";
      case socket_errc::bad_file_descriptor:
        return "bad fd";
      case socket_errc::bad_read_buffer:
        return "bad read buf";
      case socket_errc::interrupted:
        return "interrupted";
      case socket_errc::invalid_descriptor:
        return "invalid fd";
      case socket_errc::io_error:
        return "io error";
      case socket_errc::addr_in_use:
        return "address in use";
      case socket_errc::conn_refused:
        return "connection refused";
      case socket_errc::in_progress:
        return "operation in progress";
      case socket_errc::unknown:
        return "unknown";
      default:
        assert (0 && "Code not reached");
    };
  }
};

// Create global object for the error categories
const socket_err_category the_socket_err_cat{};

} // END anonymous namespace


inline std::error_code make_error_code(socket_errc errc)
{
  return { static_cast<int>(errc), the_socket_err_cat };
}

} // END namespace error
} // END namespace coro_async

#endif
