#pragma once
#include "EventHandler.h"
#include "EventData.h"
#include <map>

namespace sevent
{
namespace socket
{

class EventHandlerCollection
{
    public:
        EventHandlerCollection();
        virtual ~EventHandlerCollection();
        void addEventHandler(EventData::eventId_t eventid, eventHandler_t eventHandler);
        void triggerEvent(EventData eventData);

    private:
        std::map<EventData::eventId_t, eventHandler_t> _eventHandlers;
};

}
