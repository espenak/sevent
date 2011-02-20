#pragma once
#include "EventHandler.h"
#include "sevent/socket/EventData.h"
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
                void addEventHandler(socket::EventData::eventId_t eventid, eventHandler_t eventHandler);
                void triggerEvent(socket::EventData eventData);

            private:
                std::map<socket::EventData::eventId_t, eventHandler_t> _eventHandlers;
        };

    } // namespace event
} // namespace sevent
