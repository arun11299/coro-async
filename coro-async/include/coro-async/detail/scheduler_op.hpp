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

#ifndef CORO_ASYNC_SCHEDULER_OP_HPP
#define CORO_ASYNC_SCHEDULER_OP_HPP

#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 * Handler for operations posted on the scheduler.
 */
template <typename Handler>
class scheduler_op: public operation_base
{
public:
  /**
   * Constructor.
   * \param ch - The completion handler.
   */
  scheduler_op(Handler&& ch)
    : operation_base(scheduler_op<Handler>::complete)
    , ch_(std::forward<Handler>(ch))
  {
  }

  scheduler_op(const scheduler_op&) = default;
  scheduler_op& operator=(const scheduler_op&) = default;

public:
  /// Invokes the completion handler.
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    auto self = static_cast<scheduler_op<Handler>*>(op);
    self->ch_();

    return;
  }

private:
  /// The user handler to be executed
  Handler ch_;
};

} // END namespace detail
} // END namespace coro_async

#endif
