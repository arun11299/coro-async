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

#ifndef CORO_ASYNC_EPOLL_REACTOR
#define CORO_ASYNC_EPOLL_REACTOR

#include "coro-async/detail/epoll.hpp"
#include "coro-async/detail/descriptor.hpp"
#include "coro-async/detail/reactor_ops.hpp"
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 * A thin wrapper for executing operations on epoll
 * descriptor.
 */
class epoll_reactor
{
public:
  /// Default constructor.
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
   *
   * \param d - The descriptor on which a non blocking operation is to be scheduled.
   * \param dstate - The corresponding descriptor state.
   * \param op - The type of operation on the descriptor.
   * \param cb - The operation callback when the descriptor is ready.
   */
  int start_op(descriptor& d, descriptor_state* dstate, reactor_ops op, operation_base* cb);

  /**
   * Registers a descriptor for the first time to
   * the event system.
   * The descriptor is registered for Input events.
   *
   * \param d - The descriptor to be registered with epoll.
   * \param dstate - The descriptor state to be initialized.
   */
  int register_descriptor(descriptor& d, descriptor_state** dstate);

  /**
   * Gather the ready events from the registered
   * descriptor set.
   *
   * \param timeout - The timeout to be used for the epoll_wait call.
   */
  void run(int timeout);

private:
  /// The epoll descriptor.
  Epoll epoll_;
};

} // END namespace detail
} // END namespace coro-async

#include "coro-async/detail/impl/epoll_reactor.ipp"

#endif
