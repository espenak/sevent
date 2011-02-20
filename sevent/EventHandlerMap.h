#pragma once
#include "socket.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

namespace sevent
{
    namespace event
    {
        class HandlerMap;
        typedef boost::shared_ptr<HandlerMap> HandlerMap_ptr;

        class HandlerMap
        {
            public:
                typedef boost::function<void(socket::Facade_ptr facade,
                                             socket::Session_ptr session,
                                             socket::ReceiveEvent& event)> handler_t;
            private:
                struct Handler
                {
                    handler_t handler;
                };
                typedef boost::shared_ptr<Handler> Handler_ptr;

            public:
                HandlerMap();
                virtual ~HandlerMap();
                static HandlerMap_ptr make();
            public:
                void addEventHandler(socket::ReceiveEvent::eventId_t eventid,
                                     handler_t handler);
                void triggerEvent(socket::Facade_ptr facade,
                                  socket::Session_ptr session,
                                  socket::ReceiveEvent& event);

            private:
                std::map<socket::ReceiveEvent::eventId_t, Handler_ptr> _handlers;
                boost::mutex _lock;
        };

    } // namespace event
} // namespace sevent
