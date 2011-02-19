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
    _sessions.push_back(session);
}


void SocketSessionRegistry::remove(SocketSession_ptr session)
{
    std::cout << "Removed!" << std::endl;
}

} // namespace socket
} // namespace sevent
