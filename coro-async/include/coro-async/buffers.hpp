#ifndef CORO_ASYNC_BUFFERS_HPP
#define CORO_ASYNC_BUFFERS_HPP

#include <string>
#include <vector>

namespace coro_async {
namespace buffer {

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
} // END namespace coro-async

#endif
