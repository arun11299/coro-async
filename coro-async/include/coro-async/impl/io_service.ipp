#ifndef CORO_ASYNC_IO_SERVICE_IPP
#define CORO_ASYNC_IO_SERVICE_IPP

namespace coro_async {

void io_service::run()
{
  while ( true )
  {
    reactor_.run(2);

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
