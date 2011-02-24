#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <stdint.h>
#include <string>
#include "Address.h"
#include "ReceiveEvent.h"
#include "ConstBuffer.h"

namespace sevent
{
    namespace socket
    {

        class Session;
        typedef boost::shared_ptr<Session> Session_ptr;

        class Session
        {
            public:
                typedef boost::function<void(Session_ptr socketSession,
                                             ReceiveEvent& event)> allEventsHandler_t;
                typedef boost::function<void(Session_ptr socketSession)>
                disconnectHandler_t;
            public:
                virtual ~Session();

                /** Send an event with a single data buffer.
                 * Blocks until the event and all it's data is completely sent. */
                virtual void sendEvent(unsigned eventid, const ConstBuffer& data) = 0;

                /** Send an event with several data buffers.
                 * Blocks until the event and all it's data is completely sent. */
                virtual void sendEvent(unsigned eventid, const ConstBufferVector& dataBufs) = 0;

                /** Add a asynchrone event-listener.
                 * This method should return at once, and make the
                 * workerthreads receive the event. */
                virtual void receiveEvents() = 0;
                
                virtual Address_ptr getLocalEndpointAddress() = 0;
                virtual Address_ptr getRemoteEndpointAddress() = 0;

            public:
                void setAllEventsHandler(allEventsHandler_t allEventHandler);
                void setDisconnectHandler(disconnectHandler_t disconnectHandler);

            protected:
                static void defaultAllEventsHandler(Session_ptr socketSession,
                                                    ReceiveEvent& event);
            protected:
                allEventsHandler_t _allEventsHandler;
                disconnectHandler_t _disconnectHandler;
        };

    } // namespace socket
} // namespace sevent
