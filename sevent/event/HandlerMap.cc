#include "HandlerMap.h"
#include <boost/lexical_cast.hpp>
#include <iostream>

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

        bool HandlerMap::contains(eventid_t eventid)
        {
            boost::lock_guard<boost::mutex> lock(_lock);
            return _handlers.find(eventid) != _handlers.end();
        }

        void HandlerMap::addEventHandler(eventid_t eventid,
                                         handler_t handler)
        {
            Handler_ptr ev = Handler_ptr(new Handler());
            ev->handler = handler;
            boost::lock_guard<boost::mutex> lock(_lock);
            _handlers[eventid] = ev;
        }

        void HandlerMap::triggerEvent(socket::Facade_ptr facade,
                                      socket::Session_ptr session,
                                      Event_ptr event)
        {
            Handler_ptr handler;
            {
                boost::lock_guard<boost::mutex> lock(_lock);
                handler = _handlers[event->eventid()];
            }
            handler->handler(facade, session, event);
        }



        void simpleAllEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                                    socket::Facade_ptr facade,
                                    socket::Session_ptr session,
                                    Event_ptr event)
        {
            //std::cerr << "Handling event: " << event->eventid() << std::endl;
            if(eventHandlerMap->contains(event->eventid()))
            {
                //std::cerr << "Event " << event->eventid() << " has a handler!" << std::endl;
                eventHandlerMap->triggerEvent(facade, session, event);
            }
            else
            {
                throw std::runtime_error("No event handler for eventid " +
                                         boost::lexical_cast<std::string>(event->eventid()));
            }
        }

    } // namespace event
} // namespace sevent
