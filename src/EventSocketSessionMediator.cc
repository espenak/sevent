#include "EventSocketSessionMediator.h"

namespace sevent
{
namespace socket
{

EventSocketSessionMediator::EventSocketSessionMediator(
        SocketSession_ptr socketSession) :
    _socketSession(socketSession)
{
}

EventSocketSessionMediator::~EventSocketSessionMediator()
{
}

EventSocketSessionMediator_ptr EventSocketSessionMediator::make(
        SocketSession_ptr socketSession)
{
    return EventSocketSessionMediator_ptr(new EventSocketSessionMediator(
            socketSession));
}

void EventSocketSessionMediator::sendEvent(EventData eventData)
{
    _socketSession->sendEvent(eventData);
}

}
