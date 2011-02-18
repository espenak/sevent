#pragma once

#include "EventData.h"

namespace socketevent
{

class EventSocketSessionMediator
{
    public:
        EventSocketSessionMediator();
        virtual ~EventSocketSessionMediator();
        void sendEvent(EventData eventData);
};

}
