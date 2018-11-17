#ifndef CORO_ASYNC_SOCKET_IPP
#define CORO_ASYNC_SOCKET_IPP

#include "coro-async/detail/read_op.hpp"
#include "coro-async/detail/write_op.hpp"
#include "coro-async/detail/reactor_ops.hpp"
#include "coro-async/detail/descriptor_op.hpp"

namespace coro_async {

template <typename CompletionHandler>
void stream_socket::async_connect(endpoint ep, CompletionHandler&& ch)
{
  using handler_type = typename std::decay_t<CompletionHandler>;

  if (!is_open())
  {
    std::error_code ec{};
    if (!open(ec)) return;
    //TODO: Call completion handler with error
  }

  auto op = new detail::descriptor_op<handler_type>{std::forward<CompletionHandler>(ch)};
  start_reactor_op(reactor_ops::connect_op, op);

  return;
}

template <typename Buffer, typename ReadHandler>
void stream_socket::async_read_some(const Buffer& buf, ReadHandler&& rh)
{
  using handler_type = typename std::decay_t<ReadHandler>;

  if (!is_open())
  {
    std::error_code ec{};
    if (!open(ec)) return;
    //TODO: Call completion handler with error
    assert (0 && "Not implemented");
  }

  auto op = new detail::read_op<handler_type>{*this, buf, std::forward<ReadHandler>(rh)};
  start_reactor_op(reactor_ops::read_op, op);

  return;
}

template <typename ReadHandler>
void stream_socket::async_read(buffer::buffer_ref& buf, ReadHandler&& rh)
{
  using handler_type = typename std::decay_t<ReadHandler>;

  if (!is_open())
  {
    std::error_code ec{};
    if (!open(ec)) return;
    //TODO: Call completion handler with error
    assert (0 && "Not implemented");
  }

  auto op = new detail::read_op<detail::composed_read_op<handler_type>>{
                 *this, buf,
                 detail::composed_read_op<handler_type>{*this, buf, std::forward<ReadHandler>(rh)}};

  start_reactor_op(reactor_ops::read_op, op);

  return;
}

template <typename Buffer, typename WriteHandler>
void stream_socket::async_write_some(const Buffer& buf, WriteHandler&& wh)
{
  using handler_type = typename std::decay_t<WriteHandler>;

  if (!is_open())
  {
    std::error_code ec{};
    if (!open(ec)) return;
    //TODO: Call completion handler with error
    assert (0 && "Not implemented");
  }

  auto op = new detail::write_op<handler_type>{*this, buf, std::forward<WriteHandler>(wh)};
  start_reactor_op(reactor_ops::write_op, op);

  return;
}

template <typename WriteHandler>
void stream_socket::async_write(buffer::buffer_ref& buf, WriteHandler&& wh)
{
  using handler_type = typename std::decay_t<WriteHandler>;

  if (!is_open())
  {
    std::error_code ec{};
    if (!open(ec)) return;
    //TODO: Call completion handler with error
    assert (0 && "Not implemented");
  }

  auto op = new detail::write_op<detail::composed_write_op<handler_type>>{
                 *this, buf,
                 detail::composed_write_op<handler_type>{*this, buf, std::forward<WriteHandler>(wh)}};

  start_reactor_op(reactor_ops::write_op, op);

  return;
}

} // END namespace coro-async

#endif
