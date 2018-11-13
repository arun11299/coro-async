#ifndef CORO_ASYNC_IO_SERVICE_IPP
#define CORO_ASYNC_IO_SERVICE_IPP

#include <type_traits>
#include "coro-async/detail/scheduler_op.hpp"

namespace coro_async {

template <typename Handler>
void io_service::post(Handler&& h)
{
  using handler_type = typename std::decay_t<Handler>;

  auto op = new detail::scheduler_op<handler_type>{std::forward<Handler>(h)};
  scheduler_.post(op);
}

void io_service::run()
{
  while ( true )
  {
    std::error_code ec{};
    scheduler_.run(ec);

    // Check for timer tasks
    auto curr_time = timers_.current_time();
    while (timers_.size())
    {
      // TODO: Copy ?
      auto element = timers_.peek();
      if (curr_time >= element.first)
      {
        (element.second)();
        timers_.remove();
      }
      else
      {
        break;
      }
    }
  }
}

} // END namespace coro_async

#endif
