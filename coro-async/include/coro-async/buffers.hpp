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

#ifndef CORO_ASYNC_BUFFERS_HPP
#define CORO_ASYNC_BUFFERS_HPP

#include <string>
#include <vector>
#include "coro-async/buffer_ref.hpp"

namespace coro_async {
namespace buffer {

/**
 * A buffer is currently simply represented as a vector of char.
 * There are many efficient ways to implement a buffer as done
 * in asio and beast.
 * TODO: Need better buffer implementation.
 */
using Buffer = std::vector<char>;

namespace helpers {

std::string to_string(const Buffer& buf)
{
  return { buf.data(), buf.size() };
}

Buffer from_string(const std::string& str)
{
  Buffer buf;
  buf.reserve(str.size());
  buf.assign(std::begin(str), std::end(str));

  return buf;
}

} // END namespace helpers
} // END namespace buffer

/**
 * Get a buffer_ref from vector<char>
 */
buffer::buffer_ref as_buffer(std::vector<char>& b)
{
  return buffer::buffer_ref{ &b[0], b.size() };
}

/**
 * Get a buffer_ref from string.
 * NOTE: String must have reserved the memory.
 */
buffer::buffer_ref as_buffer(std::string& b)
{
  return buffer::buffer_ref{ &b[0], b.size() };
}

/**
 * Get a buffer_ref from static array.
 */
template <int N>
buffer::buffer_ref as_buffer(char (&b)[N])
{
  return buffer::buffer_ref{ &b[0], N };
}

} // END namespace coro-async

#endif
