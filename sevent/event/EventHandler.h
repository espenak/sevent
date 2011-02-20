#pragma once
#include <boost/function.hpp>
#include "sevent/socket/EventData.h"
#include "EventSocketSessionMediator.h"

namespace sevent
{
    namespace event
    {

        typedef boost::function<void(EventSocketSessionMediator_ptr mediator,
                                     socket::EventData eventData)> eventHandler_t;

    } // namespace event
} // namespace sevent
