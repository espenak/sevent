#pragma once
#include <boost/function.hpp>
#include "EventSocketSessionMediator.h"

namespace socketevent
{

typedef boost::function<void (EventSocketSessionMediator mediator, EventData eventData)> eventHandler_t;

} // namespace socketevent
