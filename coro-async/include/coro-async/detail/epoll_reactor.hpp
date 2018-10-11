#ifndef CORO_ASYNC_EPOLL_REACTOR
#define CORO_ASYNC_EPOLL_REACTOR

#include "coro-async/detail/epoll.hpp"
#include "coro-async/detail/descriptor.hpp"
#include "coro-async/detail/reactor_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 */
class epoll_reactor
{
public:
  /**
   */
  explicit epoll_reactor() = default;

  /// non copyable non assignable
  epoll_reactor(const epoll_reactor&) = delete;
  epoll_reactor& operator=(const epoll_reactor&) = delete;

  ~epoll_reactor() = default;

public:
  /**
   * Modifies the events that the descriptor
   * is interested in. The interest is made known
   * by the use of `op` argument.
   */
  int start_op(descriptor& d, descriptor_state* dstate, reactor_ops op, operation_base* cb);

  /**
   * Registers a descriptor for the first time to
   * the event system.
   * The descriptor is registered for Input events.
   */
  int register_descriptor(descriptor& d, descriptor_state** dstate);

  /**
   * Gather the ready events from the registered
   * descriptor set.
   */
  void run(int timeout);

private:
  ///
  Epoll epoll_;
};

} // END namespace detail
} // END namespace coro-async

#include "coro-async/detail/impl/epoll_reactor.ipp"

#endif
