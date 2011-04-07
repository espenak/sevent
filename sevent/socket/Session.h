#pragma once
#include <boost/function.hpp>
#include <stdint.h>
#include <string>
#include "SessionRegistryForwardDecl.h"
#include "Address.h"
#include "sevent/event/Event.h"

namespace sevent
{
    namespace socket
    {
        class Session;
        typedef boost::shared_ptr<Session> Session_ptr;
        class Session
        {
            friend class SessionRegistry; // enable it to close() when removing sessions
            public:
                typedef boost::function<void(Session_ptr socketSession,
                                             event::Event_ptr event)> allEventsHandler_t;
                typedef boost::function<void(Session_ptr socketSession)> disconnectHandler_t;
            public:
                virtual ~Session();

                /** Send an event with no data buffer.
                 * Blocks until the event is completely sent. */
                //virtual void sendEvent(unsigned eventid) = 0;

                /** Send an event with a single data buffer.
                 * Blocks until the event and all it's data is completely sent. */
                virtual void sendEvent(event::Event_ptr event) = 0;

                /** Add a asynchrone event-listener.
                 * This method should return at once, and make the
                 * workerthreads receive the event. */
                virtual void receiveEvents() = 0;

                
                virtual Address_ptr getLocalEndpointAddress() = 0;
                virtual Address_ptr getRemoteEndpointAddress() = 0;

            public:
                /** Set the disconnect handler. Normally forwarded/set in
                 * SessionRegistry::setDisconnectHandler(). */
                void setDisconnectHandler(disconnectHandler_t disconnectHandler);

                /** Set the all-events-handler. Normally forwarded/set in
                 * SessionRegistry::setAllEventsHandler(). The default
                 * handler is defaultAllEventsHandler(). */
                void setAllEventsHandler(allEventsHandler_t allEventHandler);
            protected:
                virtual void close() = 0;
            public:
                /** Print some debugging info about the event. */
                static void defaultAllEventsHandler(Session_ptr socketSession,
                                                    event::Event_ptr event);
            protected:
                allEventsHandler_t _allEventsHandler;
                disconnectHandler_t _disconnectHandler;
        };

    } // namespace socket
} // namespace sevent
