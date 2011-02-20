#pragma once
#include <boost/function.hpp>
#include "sevent/socket/ReceiveEvent.h"
#include "EventSessionMediator.h"

namespace sevent
{
    namespace event
    {

        typedef boost::function<void(EventSessionMediator_ptr mediator,
                                     socket::ReceiveEvent eventData)> eventHandler_t;

    } // namespace event
} // namespace sevent
