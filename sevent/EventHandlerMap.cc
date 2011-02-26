#include "EventHandlerMap.h"
#include <boost/lexical_cast.hpp>

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

        bool HandlerMap::contains(socket::ReceiveEvent::eventId_t eventid)
        {
            boost::lock_guard<boost::mutex> lock(_lock);
            return _handlers.find(eventid) != _handlers.end();
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



        void simpleAllEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                                    socket::Facade_ptr facade,
                                    socket::Session_ptr session,
                                    socket::ReceiveEvent& event)
        {
            if(eventHandlerMap->contains(event.eventid()))
            {
                eventHandlerMap->triggerEvent(facade, session, event);
            }
            else
            {
                throw std::runtime_error("No event handler for eventid " +
                                         boost::lexical_cast<std::string>(event.eventid()));
            }
        }

    } // namespace event
} // namespace sevent
