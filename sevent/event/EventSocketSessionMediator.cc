#include "EventSocketSessionMediator.h"

namespace sevent
{
    namespace event
    {

        EventSocketSessionMediator::EventSocketSessionMediator(
            socket::SocketSession_ptr socketSession) :
            _socketSession(socketSession)
        {
        }

        EventSocketSessionMediator::~EventSocketSessionMediator()
        {
        }

        EventSocketSessionMediator_ptr EventSocketSessionMediator::make(
            socket::SocketSession_ptr socketSession)
        {
            return EventSocketSessionMediator_ptr(new EventSocketSessionMediator(
                    socketSession));
        }

        void EventSocketSessionMediator::sendEvent(socket::EventData eventData)
        {
            _socketSession->sendEvent(eventData);
        }

    } // namespace event
} // namespace sevent
