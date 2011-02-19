#include "SocketSessionRegistry.h"
#include <boost/bind.hpp>
#include <iostream>

namespace sevent
{
namespace socket
{

SocketSessionRegistry::SocketSessionRegistry()
{
}

SocketSessionRegistry::SocketSessionRegistry(
        SocketSession::allEventsHandler_t allEventHandler) :
    _allEventsHandler(allEventHandler)
{
}

SocketSessionRegistry::~SocketSessionRegistry()
{
}

SocketSessionRegistry_ptr SocketSessionRegistry::make()
{
    return SocketSessionRegistry_ptr(new SocketSessionRegistry());
}

SocketSessionRegistry_ptr SocketSessionRegistry::make(
        SocketSession::allEventsHandler_t allEventHandler)
{
    return SocketSessionRegistry_ptr(new SocketSessionRegistry(allEventHandler));
}

void SocketSessionRegistry::add(SocketSession_ptr session)
{
    if (_allEventsHandler)
    {
        session->setAllEventsHandler(_allEventsHandler);
    }
    session->setDisconnectHandler(boost::bind(&SocketSessionRegistry::remove,
            this, _1));
    std::string remoteAddr = session->getRemoteEndpointAddress()->str();
    _sessions[remoteAddr] = session;
    std::cout << "Added " << remoteAddr << std::endl;
}


void SocketSessionRegistry::remove(SocketSession_ptr session)
{

    std::string remoteAddr = session->getRemoteEndpointAddress()->str();
    _sessions.erase(remoteAddr);
    std::cout << "Removed " << remoteAddr << std::endl;
}

} // namespace socket
} // namespace sevent
