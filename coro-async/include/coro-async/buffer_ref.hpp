#ifndef CORO_ASYNC_buffer_ref_HPP
#define CORO_ASYNC_buffer_ref_HPP

#include <cassert>

namespace coro_async {
namespace buffer     {

class buffer_ref
{
public:
  /// 
  explicit buffer_ref()
  {
  }

  ///
  explicit buffer_ref(char* ptr, size_t len)
    : ptr_(ptr)
    , size_(len)
  {
  }

  // Copyable
  buffer_ref(const buffer_ref& other)
    : ptr_(other.ptr_)
    , size_(other.size_)
  {
  }

  // Move copyable
  buffer_ref(buffer_ref&& other) noexcept
    : ptr_(other.ptr_)
    , size_(other.size_)
  {
    other.ptr_ = nullptr;
    other.size_ = 0;
  }

  // Copy assignable
  buffer_ref& operator=(const buffer_ref& other)
  {
    ptr_ = other.ptr_;
    size_ = other.size_;

    return *this;
  }

  // Move assignable
  buffer_ref& operator=(buffer_ref&& other) noexcept
  {
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;

    size_ = other.size_;
    other.size_ = 0;

    return *this;
  }

  ~buffer_ref()
  {
    ptr_ = nullptr;
    size_ = 0;
  }

public: // Exposed APIs

  /// Get the pointed data
  char* data() noexcept
  {
    return ptr_;
  }

  const char* data() const noexcept
  {
    return ptr_;
  }

  /// Get the size
  size_t size() const noexcept
  {
    return size_;
  }

  /// Consume `n` bytes from the buffer
  void consume(size_t n)
  {
    assert (n <= size_ && "Buffer overflow");
    ptr_ += n;
    size_ -= n;
  }

private:
  /// The pointer to data (Doesn't own)
  char* ptr_ = nullptr;
  /// Size of the pointed buffer
  size_t size_ = 0;
};

} // END namespace buffer
} // END namespace coro_async

#endif
