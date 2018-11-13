#ifndef CORO_ASYNC_OPERATION_QUEUE_HPP
#define CORO_ASYNC_OPERATION_QUEUE_HPP

#include <cassert>
#include "coro-async/detail/operation_base.hpp"

namespace coro_async {

/**
 */
template <typename Operation>
class operation_queue
{
public:
  ///
  operation_queue() = default;

  /// Non copyable and non assignable
  operation_queue(const operation_queue&) = delete;
  operation_queue& operator=(const operation_queue&) = delete;

  ~operation_queue()
  {
    while (head_)
    {
      pop();
    }
  }

public: // Queue APIs

  /// Return the first element in the queue
  Operation* head() noexcept
  {
    return head_;
  }

  /// Return the last element in the queue
  Operation* tail() noexcept
  {
    return tail_;
  }

  /// Check if queue is empty
  bool is_empty() const noexcept
  {
    return head_ == nullptr &&  tail_ == nullptr;
  }

  /// Get and remove the first element
  Operation* pop() noexcept
  {
    assert (!is_empty() && "Pop called on empty queue");

    auto first = head_;
    head_ = head_->next_;
    first->next_ = nullptr;

    // Did we just pop out the last element
    if (!head_)
    {
      tail_ = nullptr;
    }

    return first;
  }

  /// Add an element to the queue
  void push(Operation* op) noexcept
  {
    if (!tail_)
    {
      assert (!head_ && "Head cannot be non-null when tail is null");
      head_ = tail_ = op;
    }
    else
    {
      assert (!tail_->next_ && "tail operation cannot point to anything");
      tail_->next_ = op;
    }
  }

  /// Push an entire queue
  template <typename OtherOperation>
  void push(operation_queue<OtherOperation>& oq)
  {
    if (oq.is_empty()) return;

    if (is_empty())
    {
      head_ = oq.head_;
      tail_ = oq.tail_;
      return;
    }

    oq.tail_->next_ = head_;
    head_ = oq.head_;
  }

  /// Checks if the operation is enqueued
  bool is_enqueued(Operation* op) const noexcept
  {
    return (op->next_ != nullptr) || (tail_ == op);
  }
  
private:
  ///
  Operation* head_ = nullptr;
  ///
  Operation* tail_ = nullptr;
};

} // END namespace coro_async

#endif
