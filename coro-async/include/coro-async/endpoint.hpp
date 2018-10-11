#ifndef CORO_ASYNC_ENDPOINT_HPP
#define CORO_ASYNC_ENDPOINT_HPP

#include "coro-async/ip_address.hpp"

namespace coro_async {

/**
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
