#include "SocketSession.h"
#include <iostream>

namespace socketevent
{

SocketSession::~SocketSession()
{
}

void SocketSession::defaultAllEventsHandler(
        SocketSession_ptr socketSession, EventData eventData)
{
    std::cout
            << "Event received by default all-events handler. "
            << "Use SocketSession::setAllEventsHandler to plug in your own." << std::endl
            << "Event id: " << eventData.eventid() << std::endl
            << "Event data size: " << eventData.dataSize() << std::endl
            << "Event data: ";
    std::cout.write(eventData.data(), eventData.dataSize());
    std::cout << std::endl;
}

void SocketSession::setAllEventsHandler(allEventsHandler_t allEventsHandler)
{
    _allEventsHandler = allEventsHandler;
}

}
