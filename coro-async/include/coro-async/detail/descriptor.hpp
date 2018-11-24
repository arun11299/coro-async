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

#ifndef CORO_ASYNC_DESCRIPTOR_HPP
#define CORO_ASYNC_DESCRIPTOR_HPP

#include <queue>
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 * A thin wrapper for socket descriptors.
 * The descriptors added are *always* made non-blocking.
 * Movable only type.
 *
 * NOTE: Not validation is done to make sure than only
 * socket descriptors are handled by this type.
 */
class descriptor
{
public:
  /// The underlying type of the descriptor (OS dependent).
  using descriptor_type = int;

public: // 'tors
  /// Default constructor.
  descriptor() = default;

  /// Create a descriptor object from the
  /// native file descriptor.
  descriptor(descriptor_type d)
    : fd_(d)
  {
    make_fd_non_blocking();
  }

  /// Move constructor.
  descriptor(descriptor&& other)
    : fd_(other.fd_)
  {
    other.fd_ = -1;
    make_fd_non_blocking();
  }

  /// Copy constructor(deleted).
  descriptor(const descriptor&) = delete;

  /// Move assignment.
  descriptor& operator=(descriptor&& other)
  {
    if (fd_ != -1) close(fd_);
    fd_ = other.fd_;
    other.fd_ = -1;
    make_fd_non_blocking();
    return *this;
  }

  /// Copy assignment(deleted).
  descriptor& operator=(const descriptor&) = delete;

  /// Close the descriptor
  ~descriptor()
  {
    if (fd_ != -1) close(fd_);
  }

public: // Public APIs
  /**
   * Returns the underlying the descriptor.
   */
  int get() const noexcept
  {
    return fd_;
  }

  /**
   * Use this API cautiously. The ownership of the
   * file descriptor not belongs to descriptor object.
   */
  void set(descriptor_type fd) noexcept
  {
    if (fd_ != -1) close(fd_);
    fd_ = fd;
    make_fd_non_blocking();
  }

private:
  /// Make the descriptor non-blocking.
  void make_fd_non_blocking()
  {
    assert (fd_ != -1);
    auto flags = fcntl(fd_, F_GETFL);
    flags |= O_NONBLOCK;

    fcntl(fd_, F_SETFL, flags);
  }

private:
  /// The real nateive file descriptor
  descriptor_type fd_ = -1;
};


/**
 * Stores the operations pending on a `descriptor`.
 * An instance of this class is what is passed down as a
 * context to the epoll event.
 */
class descriptor_state
{
public:
  /// Queue of operations.
  /// TODO: Make use of intrusive operation queue.
  using op_queue = std::queue<operation_base*>;

public:
  /// Constructor.
  /// \param  desc - The descriptor for which the operations
  ///                are stored.
  /// NOTE: The descriptor desc should be valid for the 
  /// lifetime of the descriptor state.
  descriptor_state(descriptor& desc)
    : desc_(desc)
  {
  }

  /// non copyable and non assignable
  descriptor_state(const descriptor_state&) = delete;
  descriptor_state& operator=(const descriptor_state&) = delete;

  ~descriptor_state() = default;

public:
  /**
   */
  void set_ready_events(uint32_t recv_events)
  {
  }

  /**
   * Push the operation to the passed queue.
   * \param op - The operation to be added.
   * \param q - The queue to which the operation to be added.
   *            The passed queue *should* be reference to
   *            one of rd/wr/connect queues.
   */
  void push_op(operation_base* op, op_queue& q)
  {
    q.push(op);
    return;
  }

  /**
   * Check to see if the passed queue is empty or not.
   * \param q - The operation queue.
   *            The passed queue *should* be reference to
   *            one of rd/wr/connect queues.
   */
  bool is_op_queue_empty(const op_queue& q) const noexcept
  {
    return q.empty();
  }

  /**
   * Get the first operation in the queue.
   * \param q - The operation queue.
   *            The passed queue *should* be reference to
   *            one of rd/wr/connect queues.
   */
  operation_base* pop_front_op(op_queue& q)
  {
    assert (!is_op_queue_empty(q));

    auto op = q.front();
    q.pop();
    return op;
  }

  /// Get reference to the read operation queue.
  op_queue& rd_q() noexcept { return rd_op_queue_; }

  /// Get reference to the write operation queue.
  op_queue& wr_q() noexcept { return wr_op_queue_; }

  /// Get reference to the connect operation queue.
  op_queue& connect_q() noexcept { return co_op_queue_; }

public:
  /// Registered epoll events
  uint32_t registered_events_ = 0;

private:
  /// The associated descriptor
  descriptor& desc_;
  /// Queue of pending read operations
  std::queue<operation_base*> rd_op_queue_;
  /// Queue of pending write operations
  std::queue<operation_base*> wr_op_queue_;
  /// Queue of pending connect operations
  std::queue<operation_base*> co_op_queue_;
};

} // END namespace detail
} // END namespace coro-async

#endif
