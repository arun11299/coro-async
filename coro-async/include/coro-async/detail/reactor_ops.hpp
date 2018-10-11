#ifndef CORO_ASYNC_REACTOR_OPS_HPP
#define CORO_ASYNC_REACTOR_OPS_HPP

/**
 */
enum class reactor_ops
{
  connect_op = 0,
  write_op   = 1,
  read_op    = 2,
  except_op  = 3,
  max_ops    = 4,
};

#endif
