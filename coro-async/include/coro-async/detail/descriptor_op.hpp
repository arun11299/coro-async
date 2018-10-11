#ifndef CORO_ASYNC_DESCRIPTOR_OP_HPP
#define CORO_ASYNC_DESCRIPTOR_OP_HPP

#include <cassert>
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 */
template <typename Handler>
class descriptor_op: public operation_base
{
public:
  /**
   */
  descriptor_op(Handler&& h)
    : operation_base(descriptor_op<Handler>::complete)
    , handler_(std::forward<Handler>(h))
  {
  }

  /**
   */
  static void complete(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    assert (op);
    auto self = static_cast<descriptor_op<Handler>*>(op);
    self->handler_(ec, bytes_xferred);
    return;
  }

private:
  ///
  Handler handler_;
};

} // END namespace detail
} // END namespace coro-async

#endif
