#include "Session.h"
#include <iostream>

namespace sevent
{
    namespace socket
    {

        Session::~Session()
        {
        }

        void Session::setDisconnectHandler(disconnectHandler_t disconnectHandler)
        {
            _disconnectHandler = disconnectHandler;
        }

        void Session::defaultAllEventsHandler(Session_ptr socketSession,
                event::Event_ptr event)
        {
            std::cout << "Event received by default all-events handler. "
                      << std::endl << "Event id: " << event->eventid() << std::endl;
        }

        void Session::setAllEventsHandler(allEventsHandler_t allEventsHandler)
        {
            _allEventsHandler = allEventsHandler;
        }
    } // namespace socket
} // namespace sevent
