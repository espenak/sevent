#include "EventSessionMediator.h"

namespace sevent
{
    namespace event
    {

        EventSessionMediator::EventSessionMediator(
            socket::Session_ptr socketSession) :
            _socketSession(socketSession)
        {
        }

        EventSessionMediator::~EventSessionMediator()
        {
        }

        EventSessionMediator_ptr EventSessionMediator::make(
            socket::Session_ptr socketSession)
        {
            return EventSessionMediator_ptr(new EventSessionMediator(
                    socketSession));
        }

        void EventSessionMediator::sendEvent(socket::ReceiveEvent eventData)
        {
            _socketSession->sendEvent(eventData);
        }

    } // namespace event
} // namespace sevent
