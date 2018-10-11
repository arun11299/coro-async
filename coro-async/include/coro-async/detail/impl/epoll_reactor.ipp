#ifndef CORO_ASYNC_EPOLL_REACTOR_IPP
#define CORO_ASYNC_EPOLL_REACTOR_IPP

#include <cassert>
#include "coro-async/detail/reactor_ops.hpp"

namespace coro_async {
namespace detail {

int epoll_reactor::start_op(
    descriptor& d,
    descriptor_state* dstate,
    reactor_ops op,
    operation_base* cb)
{
  epoll_event ev = { 0, { 0 } };
  ev.events = EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;

  auto on_empty = [&](descriptor_state::op_queue& q)
  {
    assert (dstate->is_op_queue_empty(q));

    if (op == reactor_ops::write_op) {
      if ((dstate->registered_events_ & EPOLLOUT) == 0) {
        ev.events = dstate->registered_events_ | EPOLLOUT;
      }
    }
  };

  auto on_not_empty = [&](descriptor_state::op_queue& q)
  {
    assert (!dstate->is_op_queue_empty(q));

    if (op == reactor_ops::write_op) {
      ev.events = dstate->registered_events_ | EPOLLOUT;
    }
  };

  auto check_events = [&](descriptor_state::op_queue& q)
  {
    if (dstate->is_op_queue_empty(q)) on_empty(q);
    else on_not_empty(q);
  };

  if (dstate->registered_events_ == 0) {
    //ATTN: Handle error case
    assert (0);
    return -1;
  }
  std::queue<operation_base*>* opq = nullptr;

  switch (op)
  {
    case reactor_ops::connect_op:
    {
      ev.events |= EPOLLOUT;
      opq = &dstate->connect_q();
      check_events(*opq);
      break;
    }
    case reactor_ops::write_op:
    {
      ev.events |= EPOLLOUT;
      opq = &dstate->wr_q();
      check_events(*opq);
      break;
    }
    case reactor_ops::read_op:
    {
      ev.events |= EPOLLIN;
      opq = &dstate->rd_q();
      check_events(*opq);
      break;
    }
    default:
      assert (0 && "Code not reached");
  };

  std::error_code ec{};
  ev.data.ptr = dstate;
  epoll_.modify_descriptor(d.get(), &ev, ec);

  if (!ec)
  {
    dstate->registered_events_ |= ev.events;
    //Add the callback handler to the Op queue
    dstate->push_op(cb, *opq);
  }

  return ec ? ec.value() : 0;
}

int epoll_reactor::register_descriptor(descriptor& d, descriptor_state** dstate)
{
  assert (d.get() != -1);

  *dstate = new descriptor_state{d};

  epoll_event ev = {0, { 0 }};
  ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
  (*dstate)->registered_events_ |= ev.events;
  // The operations would be pushed in `start_op`
  ev.data.ptr = *dstate;

  std::error_code ec{};
  epoll_.add_descriptor(d.get(), &ev, ec);

  return ec ? ec.value() : 0;
}

void epoll_reactor::run(int timeout)
{
  epoll_event events[128];
  int num_events = epoll_wait(epoll_.get(),
                              &events[0],
                              sizeof(events)/sizeof(epoll_event),
                              -1);

  for (int i = 0; i < num_events; i++)
  {
    void* ptr = events[i].data.ptr;
    auto dstate = static_cast<descriptor_state*>(ptr);
    std::cout << "dstate ptr: " << (intptr_t)dstate << std::endl;

    if ((dstate->registered_events_ | EPOLLOUT))
    {
      if (dstate->connect_q().size())
      {
        auto& q = dstate->connect_q();
        auto& cb = q.front();

        std::error_code ec{};
        cb->call(cb, ec, 0);
        // Remove the callback
        q.pop();
      }

      // Check for write tasks
      if (dstate->wr_q().size())
      {
        auto& q = dstate->wr_q();
        auto& cb = q.front();

        std::error_code ec{};
        cb->call(cb, ec, 0);
        // Remove the callback
        q.pop();
      }
    }

    if ((dstate->registered_events_ | EPOLLIN))
    {
      if (dstate->rd_q().size())
      {
        auto& q = dstate->rd_q();
        auto& cb = q.front();

        std::error_code ec{};
        cb->call(cb, ec, 0);
        // Remove the callback
        q.pop();
      }
    }
  }

  return;
}

} // END namespace detail
} // END namespace coro-async

#endif
