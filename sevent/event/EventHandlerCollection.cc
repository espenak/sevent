#include "EventHandlerCollection.h"

namespace sevent
{
    namespace event
    {

        EventHandlerCollection::EventHandlerCollection()
        {
        }

        EventHandlerCollection::~EventHandlerCollection()
        {
        }

        void EventHandlerCollection::addEventHandler(socket::ReceiveEvent::eventId_t eventid,
                eventHandler_t eventHandler)
        {
        }

        void EventHandlerCollection::triggerEvent(socket::ReceiveEvent eventData)
        {
        }

    } // namespace event
} // namespace sevent
