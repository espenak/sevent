#pragma once
#include <boost/function.hpp>
#include "sevent/socket.h"

namespace sevent
{
    namespace event
    {
        typedef boost::function<void(Facade_ptr facade,
                                     Session_ptr session,
                                     ReceiveEvent& event)> allEventsHandler_t;
    } // namespace event
} // namespace sevent
