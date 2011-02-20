#pragma once
#include "EventHandler.h"
#include "sevent/socket/ReceiveEvent.h"
#include <map>

namespace sevent
{
    namespace event
    {

        class EventHandlerCollection
        {
            public:
                EventHandlerCollection();
                virtual ~EventHandlerCollection();
                void addEventHandler(socket::ReceiveEvent::eventId_t eventid, eventHandler_t eventHandler);
                void triggerEvent(socket::ReceiveEvent event);

            private:
                std::map<socket::ReceiveEvent::eventId_t, eventHandler_t> _eventHandlers;
        };

    } // namespace event
} // namespace sevent
