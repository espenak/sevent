#pragma once
#include <boost/function.hpp>
#include "EventData.h"
#include "EventSocketSessionMediator.h"

namespace socketevent
{

typedef boost::function<void(EventSocketSessionMediator_ptr mediator,
        EventData eventData)> eventHandler_t;

} // namespace socketevent
