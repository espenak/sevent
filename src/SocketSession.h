#pragma once
#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include "EventHandler.h"
#include "EventData.h"


namespace socketevent
{

class SocketSession
{
    public:
        virtual ~SocketSession();
        virtual void sendEvent(EventData eventData) = 0;
        virtual void reveiveEvents() = 0;
    public:
        void setEventHandler();

    private:
        void defaultEventHandler();
};

typedef boost::shared_ptr<SocketSession> SocketSession_ptr;

}
