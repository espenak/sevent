#include "SocketSession.h"
#include <iostream>

namespace sevent
{
namespace socket
{

SocketSession::~SocketSession()
{
}

void SocketSession::setDisconnectHandler(disconnectHandler_t disconnectHandler)
{
    _disconnectHandler = disconnectHandler;
}

void SocketSession::defaultAllEventsHandler(SocketSession_ptr socketSession,
        EventData eventData)
{
    std::cout << "Event received by default all-events handler. "
            << "Use SocketSession::setAllEventsHandler to plug in your own."
            << std::endl << "Event id: " << eventData.eventid() << std::endl
            << "Event data size: " << eventData.dataSize() << std::endl
            << "Event data: ";
    std::cout.write(eventData.data(), eventData.dataSize());
    std::cout << std::endl;
}

void SocketSession::setAllEventsHandler(allEventsHandler_t allEventsHandler)
{
    _allEventsHandler = allEventsHandler;
}

} // namespace socket
} // namespace sevent
