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

#ifndef CORO_ASYNC_OPERATION_BASE_HPP
#define CORO_ASYNC_OPERATION_BASE_HPP

#include <cassert>
#include <system_error>

namespace coro_async {
namespace detail {

/**
 * The base class for all operations.
 * An instance of class derived from this class is usually
 * allocated and assigned to `epoll.data.ptr` as the context.
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
   * The callback to be executed when the descriptor
   * state is ready for an operation.
   */
  operation_base(callback_t cb)
    : cb_(cb)
  {
  }

  /**
   * Call the registered callback which is usually implemented
   * by the derived class.
   */
  void call(operation_base* op, const std::error_code& ec, size_t bytes_xferred)
  {
    assert (op && "Operation cannot be nullptr");
    cb_(op, ec, bytes_xferred);
  }

private:
  /// The callback registered by the concrete implementation.
  callback_t cb_ = nullptr;

public: // TODO:: Needs an access class to be made friend of
  /// Forms a linked list
  operation_base* next_ = nullptr;
};

} // END namespace detail
} // END namespace coro-async

#endif
