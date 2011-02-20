#pragma once

#include "sevent/socket/ReceiveEvent.h"
#include "sevent/socket/Session.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace event
    {

        class EventSessionMediator;
        typedef boost::shared_ptr<EventSessionMediator>
        EventSessionMediator_ptr;

        /** Somewhat decouples eventhandlers from Session by only giving events
         * access to the functions they need.
         */
        class EventSessionMediator
        {
            public:
                static EventSessionMediator_ptr make(
                    socket::Session_ptr socketSession);
            public:
                EventSessionMediator(socket::Session_ptr socketSession);
                virtual ~EventSessionMediator();
                void sendEvent(socket::ReceiveEvent eventData);
            private:
                socket::Session_ptr _socketSession;
        };

    } // namespace event
} // namespace sevent
