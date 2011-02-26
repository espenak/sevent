#pragma once

/** \namespace sevent::event
 *
 * Makes event handling easier.
 *
 * \section Example
 *
 * \include EventExample.cc
 * */


#include "socket.h"
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace sevent
{
    namespace event
    {
        class HandlerMap;
        typedef boost::shared_ptr<HandlerMap> HandlerMap_ptr;

        /** A map of event handlers to make it easier to route events. */
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
                bool contains(socket::ReceiveEvent::eventId_t eventid);
                void addEventHandler(socket::ReceiveEvent::eventId_t eventid,
                                     handler_t handler);
                void triggerEvent(socket::Facade_ptr facade,
                                  socket::Session_ptr session,
                                  socket::ReceiveEvent& event);

            private:
                boost::unordered_map<socket::ReceiveEvent::eventId_t, Handler_ptr> _handlers;
                boost::mutex _lock;
        };

    } // namespace event
} // namespace sevent
