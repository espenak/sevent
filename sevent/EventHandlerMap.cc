#include "EventHandlerMap.h"

namespace sevent
{
    namespace event
    {
        HandlerMap::HandlerMap()
        {
        }

        HandlerMap::~HandlerMap()
        {
        }


        HandlerMap_ptr HandlerMap::make()
        {
            return HandlerMap_ptr(new HandlerMap());
        }

        void HandlerMap::addEventHandler(socket::ReceiveEvent::eventId_t eventid,
                                         handler_t handler)
        {
            Handler_ptr ev = Handler_ptr(new Handler());
            ev->handler = handler;
            boost::lock_guard<boost::mutex> lock(_lock);
            _handlers[eventid] = ev;
        }

        void HandlerMap::triggerEvent(socket::Facade_ptr facade,
                                      socket::Session_ptr session,
                                      socket::ReceiveEvent& event)
        {
            boost::lock_guard<boost::mutex> lock(_lock);
            Handler_ptr handler = _handlers[event.eventid()];
            handler->handler(facade, session, event);
        }

    } // namespace event
} // namespace sevent
