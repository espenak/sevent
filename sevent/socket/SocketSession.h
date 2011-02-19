#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <stdint.h>
#include <string>
#include "Address.h"
#include "EventData.h"

namespace sevent
{
namespace socket
{

class SocketSession;
typedef boost::shared_ptr<SocketSession> SocketSession_ptr;

class SocketSession
{
    public:
        typedef boost::function<void(SocketSession_ptr socketSession,
                EventData eventData)> allEventsHandler_t;
        typedef boost::function<void(SocketSession_ptr socketSession)>
                disconnectHandler_t;
    public:
        virtual ~SocketSession();
        virtual void sendEvent(EventData eventData) = 0;
        virtual void receiveEvents() = 0;
        virtual Address_ptr getLocalEndpointAddress() = 0;
        virtual Address_ptr getRemoteEndpointAddress() = 0;

    public:
        void setAllEventsHandler(allEventsHandler_t allEventHandler);
        void setDisconnectHandler(disconnectHandler_t disconnectHandler);

    protected:
        static void defaultAllEventsHandler(SocketSession_ptr socketSession,
                EventData eventData);
    protected:
        allEventsHandler_t _allEventsHandler;
        disconnectHandler_t _disconnectHandler;
};

} // namespace socket
} // namespace sevent
