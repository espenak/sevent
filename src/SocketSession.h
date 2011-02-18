#pragma once
#include <boost/shared_ptr.hpp>
#include "Buffer.h"
#include "EventHandler.h"


namespace socketevent
{

class SocketSession
{
    public:
        virtual ~SocketSession();
        virtual void sendEvent(eventId_t eventid, Buffer buffer) = 0;
        virtual void reveiveEvents() = 0;
};

typedef boost::shared_ptr<SocketSession> SocketSession_ptr;

}
