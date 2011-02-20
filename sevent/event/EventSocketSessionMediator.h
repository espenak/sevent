#pragma once

#include "sevent/socket/EventData.h"
#include "sevent/socket/SocketSession.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace event
    {

        class EventSocketSessionMediator;
        typedef boost::shared_ptr<EventSocketSessionMediator>
        EventSocketSessionMediator_ptr;

        /** Somewhat decouples eventhandlers from SocketSession by only giving events
         * access to the functions they need.
         */
        class EventSocketSessionMediator
        {
            public:
                static EventSocketSessionMediator_ptr make(
                    socket::SocketSession_ptr socketSession);
            public:
                EventSocketSessionMediator(socket::SocketSession_ptr socketSession);
                virtual ~EventSocketSessionMediator();
                void sendEvent(socket::EventData eventData);
            private:
                socket::SocketSession_ptr _socketSession;
        };

    } // namespace event
} // namespace sevent
