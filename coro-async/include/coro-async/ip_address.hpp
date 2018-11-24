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

#ifndef CORO_ASYNC_IP_ADDRESS
#define CORO_ASYNC_IP_ADDRESS

#include <netinet/in.h>
#include <arpa/inet.h>
#include <array>
#include <string_view>
#include <exception>
#include <system_error>

#include "coro-async/error_codes.hpp"

namespace coro_async {

/**
 * The type of IP address.
 */
enum class IPType
{
  IPv4,
  IPv6,
};


/**
 * The IPv4 address helper class.
 */
class v4_address
{
public:
  /// The address type
  static const IPType address_type = IPType::IPv4;

  /// Raw byte representation of IPv4 address in memory
  using raw_bytes_t = std::array<unsigned char, 4>;

public:
  /**
   */
  v4_address() = default;

  /**
   */
  v4_address(const raw_bytes_t& bytes)
  {
    if (bytes[0] > 0xFF ||
        bytes[1] > 0xFF ||
        bytes[2] > 0xFF ||
        bytes[3] > 0xFF)
    {
      auto ec = std::error_code{EINVAL, std::system_category()};
      //ATTN: Is this correct error for bad IP address ?
      throw std::system_error{ec, "Invalid Argument"};
    } 
    ::memcpy(&addr_.s_addr, bytes.data(), 4);
  }

  /**
   */
  v4_address(const struct in_addr addr)
  {
    ::memcpy(&addr_.s_addr, &addr.s_addr, 4);
  }

  /**
   */
  v4_address(std::string_view address)
  {
    raw_bytes_t bytes;
    int rc = inet_pton(AF_INET, address.data(), &bytes);
    if (rc != 1)
    {
      auto ec = std::error_code{errno, error::theAddressErrorCat};
      throw std::system_error{ec, std::strerror(errno)};
    }

    ::memcpy(&addr_.s_addr, bytes.data(), 4);
  }

  /**
   */
  in_addr native_addr() const noexcept
  {
    return addr_;
  }

private:
  /// The OS address type 
  struct in_addr addr_;
};

} // END namespace coro-async


#endif
