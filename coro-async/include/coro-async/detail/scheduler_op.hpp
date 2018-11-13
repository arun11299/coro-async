#ifndef CORO_ASYNC_SCHEDULER_OP_HPP
#define CORO_ASYNC_SCHEDULER_OP_HPP


#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 */
template <typename Handler>
class scheduler_op: public operation_base
{
public:
  ///
  scheduler_op(Handler&& ch)
    : operation_base(scheduler_op<Handler>::complete)
    , ch_(std::forward<Handler>(ch))
  {
  }

  scheduler_op(const scheduler_op&) = default;
  scheduler_op& operator=(const scheduler_op&) = default;

public:
  ///
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    std::cout << "scheduler_op::complete" << std::endl;
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
