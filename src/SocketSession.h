#pragma once
#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include "EventHandler.h"


namespace socketevent
{

class SocketSession
{
    public:
        typedef uint32_t bufSize_t;
    public:
        virtual ~SocketSession();
        virtual void sendEvent(eventId_t eventid, bufSize_t bufSize, char* buffer) = 0;
        virtual void reveiveEvents() = 0;
};

typedef boost::shared_ptr<SocketSession> SocketSession_ptr;

}
