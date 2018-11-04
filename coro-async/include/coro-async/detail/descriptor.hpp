#ifndef CORO_ASYNC_DESCRIPTOR_HPP
#define CORO_ASYNC_DESCRIPTOR_HPP

#include <queue>
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {
namespace detail {

/**
 * A type representing Socket file descriptor.
 * Descriptors are made non-blocking.
 * Movable only.
 * Only for socket descriptors.
 */
class descriptor
{
public:
  using descriptor_type = int;

public: // 'tors
  ///
  descriptor() = default;

  /// Create a descriptor object from the
  // native file descriptor
  descriptor(descriptor_type d)
    : fd_(d)
  {
    make_fd_non_blocking();
  }

  /// Move
  descriptor(descriptor&& other)
    : fd_(other.fd_)
  {
    other.fd_ = -1;
    make_fd_non_blocking();
  }

  descriptor(const descriptor&) = delete;

  /// Move assign
  descriptor& operator=(descriptor&& other)
  {
    if (fd_ != -1) close(fd_);
    fd_ = other.fd_;
    other.fd_ = -1;
    make_fd_non_blocking();
    return *this;
  }

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
  /**
   */
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
 */
class descriptor_state
{
public:
  using op_queue = std::queue<operation_base*>;

public:
  /**
   */
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
    //
  }

  /**
   */
  void push_op(operation_base* op, op_queue& q)
  {
    q.push(op);
    return;
  }

  /**
   */
  bool is_op_queue_empty(const op_queue& q) const noexcept
  {
    return q.empty();
  }

  /**
   */
  operation_base* pop_front_op(op_queue& q)
  {
    auto op = q.front();
    q.pop();
    return op;
  }

  /**
   */
  op_queue& rd_q() noexcept { return rd_op_queue_; }

  /**
   */
  op_queue& wr_q() noexcept { return wr_op_queue_; }

  /**
   */
  op_queue& connect_q() noexcept { return co_op_queue_; }

public:
  /// Registered events
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
