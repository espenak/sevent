#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <stdint.h>
#include <string>
#include "Address.h"
#include "ReceiveEvent.h"
#include "SendEvent.h"

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
                                             ReceiveEvent eventData)> allEventsHandler_t;
                typedef boost::function<void(Session_ptr socketSession)>
                disconnectHandler_t;
            public:
                virtual ~Session();
                virtual void sendEvent(const SendEvent& eventData) = 0;
                virtual void receiveEvents() = 0;
                virtual Address_ptr getLocalEndpointAddress() = 0;
                virtual Address_ptr getRemoteEndpointAddress() = 0;

            public:
                void setAllEventsHandler(allEventsHandler_t allEventHandler);
                void setDisconnectHandler(disconnectHandler_t disconnectHandler);

            protected:
                static void defaultAllEventsHandler(Session_ptr socketSession,
                                                    ReceiveEvent eventData);
            protected:
                allEventsHandler_t _allEventsHandler;
                disconnectHandler_t _disconnectHandler;
        };

    } // namespace socket
} // namespace sevent
