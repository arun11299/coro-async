#ifndef CORO_ASYNC_CORO_SOCKET_HPP
#define CORO_ASYNC_CORO_SOCKET_HPP

#include "coro-async/buffer_ref.hpp"
#include "coro-async/io_service.hpp"
#include "coro-async/stream_socket.hpp"
#include "coro-async/coro/read_awaitable.hpp"

namespace coro_async {

/**
 */
class coro_socket
{
public:
  ///
  coro_socket(io_service& ios)
    : ios_(ios)
    , sock_(ios)
  {
  }

public: // The awaitables
  ///
  //TODO: This buffer_ref thing sucks beyond my imagination
  read_awaitable read(size_t bytes, buffer::buffer_ref& buf)
  {
    return read_awaitable{sock_, bytes, buf};
  }

private:
  /// The io_service
  io_service& ios_;
  /// The underlying stream socket
  stream_socket sock_;
};

} // END namespace coro_async

#endif
