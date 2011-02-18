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

void EventHandlerCollection::addEventHandler(socket::EventData::eventId_t eventid,
        eventHandler_t eventHandler)
{
}

void EventHandlerCollection::triggerEvent(socket::EventData eventData)
{
}

} // namespace event
} // namespace sevent
