#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <stdint.h>
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
    public:
        virtual ~SocketSession();
        virtual void sendEvent(EventData eventData) = 0;
        virtual void receiveEvents() = 0;
    public:
        void setAllEventsHandler(allEventsHandler_t allEventHandler);
//        void attachToSessionRegistry(SessionRegistry_ptr sessionRegistry);

    protected:
        static void defaultAllEventsHandler(SocketSession_ptr socketSession,
                EventData eventData);
    protected:
        allEventsHandler_t _allEventsHandler;
};


} // namespace socket
} // namespace sevent
