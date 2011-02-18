#include "SocketSessionRegistry.h"

namespace socketevent
{

SocketSessionRegistry::SocketSessionRegistry()
{
    // TODO Auto-generated constructor stub

}

SocketSessionRegistry::~SocketSessionRegistry()
{
    // TODO Auto-generated destructor stub
}

SocketSessionRegistry_ptr SocketSessionRegistry::make()
{
    return SocketSessionRegistry_ptr(new SocketSessionRegistry());
}

void SocketSessionRegistry::add(SocketSession_ptr session)
{
    _sessions.push_back(session);
}

}
