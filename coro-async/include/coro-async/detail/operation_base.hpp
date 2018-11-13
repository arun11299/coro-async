#ifndef CORO_ASYNC_OPERATION_BASE_HPP
#define CORO_ASYNC_OPERATION_BASE_HPP

#include <cassert>
#include <system_error>

namespace coro_async {
namespace detail {

/**
 */
class operation_base
{
public:
  /// The callback signature expected
  using callback_t = 
    void (*)(operation_base* op,
             const std::error_code& ec,
             size_t bytes_xferred);

public: //'tors
  /**
   */
  operation_base(callback_t cb)
    : cb_(cb)
  {
  }

  /**
   */
  void call(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    assert (op && "Operation cannot be nullptr");
    cb_(op, ec, bytes_xferred);
  }

private:
  /// 
  callback_t cb_ = nullptr;
  /// Forms a linked list

public: // TODO:: Needs an access class to be made friend of
  operation_base* next_ = nullptr;
};

} // END namespace detail
} // END namespace coro-async

#endif
