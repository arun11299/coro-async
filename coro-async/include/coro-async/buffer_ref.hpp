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

#ifndef CORO_ASYNC_BUFFER_REF_HPP
#define CORO_ASYNC_BUFFER_REF_HPP

#include <cassert>

namespace coro_async {
namespace buffer     {

/**
 * A very thin wrapper representing a region of valid memory.
 */
class buffer_ref
{
public:
  /// Default cons.
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
