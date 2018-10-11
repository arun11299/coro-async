#ifndef CORO_ASYNC_ERROR_CODES_HPP
#define CORO_ASYNC_ERROR_CODES_HPP

#include <cstring>
#include <system_error>

namespace coro_async {
namespace error {

/**
 */
enum class socket_errc
{
  eof = 1,             // End of stream
  try_again,           // EAGAIN
  would_block,         // EWOULDBLOCK
  bad_file_descriptor, // EBADF
  bad_read_buffer,     // EFAULT
  interrupted,         // EINTR
  invalid_descriptor,  // EINVAL
  io_error,            // EIO
  unknown,
};


/**
 */
std::error_code make_error_code(socket_errc errc);

/**
 */
struct address_error_category: std::error_category
{
  const char* name() const noexcept override
  {
    return "address_error";
  }

  /**
   * NOTE: ev is errno
   */
  std::string message(int ev) const override
  {
    return std::strerror(ev);
  }
};

const address_error_category theAddressErrorCat{};

} // END namespace error
} // END namespace coro-async

/**
 */
namespace std {

  template <>
  struct is_error_code_enum<coro_async::error::socket_errc> : true_type {};

} // END namespace std

#include "coro-async/impl/error_codes.ipp"

#endif
