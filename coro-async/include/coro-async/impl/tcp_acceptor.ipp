#ifndef CORO_ASYNC_TCP_ACCEPTOR_IPP
#define CORO_ASYNC_TCP_ACCEPTOR_IPP

#include "coro-async/detail/acceptor_op.hpp"

namespace coro_async {

template <typename CompletionHandler>
void tcp_acceptor::async_accept(stream_socket& sock, CompletionHandler&& ch)
{
  using handler_type = typename std::decay_t<CompletionHandler>;

  // Socket should not be open
  assert (!sock.is_open());

  auto op = new detail::acceptor_op<handler_type>{
                          sock, socket_, std::forward<CompletionHandler>(ch)};
  socket_.start_reactor_op(reactor_ops::read_op, op);
}

} // END namespace coro_async

#endif
