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

#ifndef CORO_ASYNC_ENDPOINT_HPP
#define CORO_ASYNC_ENDPOINT_HPP

#include "coro-async/ip_address.hpp"

namespace coro_async {

/**
 * Represents a TCP endpoint.
 * NOTE: Only IPv4 for now.
 */
class endpoint
{
public:
  ///Endpoint constructor for v4
  explicit endpoint(v4_address addr, unsigned short port)
    : address_(addr)
    , port_(port)
  {
  }

  // Default copy construction
  endpoint(const endpoint&) = default;
  endpoint(endpoint&&)      = default;

  // Default assignable
  endpoint& operator=(const endpoint&) = default;
  endpoint& operator=(endpoint&&)      = default;

public:
  /// Get the endpoint address
  v4_address address() const noexcept
  {
    return address_;
  }

  /// Get the endpoint port
  unsigned short port() const noexcept
  {
    return port_;
  }

private:
  /// The endpoint address
  //ATTN: Variant for other kinds of addresses
  v4_address address_;
  /// Endpoint port
  unsigned short port_;
};

}


#endif
