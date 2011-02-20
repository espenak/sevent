#include "SocketSessionRegistry.h"
#include <boost/bind.hpp>
#include <iostream>

namespace sevent
{
    namespace socket
    {

        SocketSessionRegistry::SocketSessionRegistry()
        {
            setDisconnectHandler(boost::bind(&SocketSessionRegistry::remove,
                                             this, _1));
        }

        SocketSessionRegistry::~SocketSessionRegistry()
        {
        }

        SocketSessionRegistry_ptr SocketSessionRegistry::make()
        {
            return SocketSessionRegistry_ptr(new SocketSessionRegistry());
        }

        void SocketSessionRegistry::add(SocketSession_ptr session)
        {
            if (_allEventsHandler)
            {
                session->setAllEventsHandler(_allEventsHandler);
            }
            session->setDisconnectHandler(_disconnectHandler);
            std::string remoteAddr = session->getRemoteEndpointAddress()->str();
            _sessions[remoteAddr] = session;
        }

        void SocketSessionRegistry::remove(SocketSession_ptr session)
        {
            std::string remoteAddr = session->getRemoteEndpointAddress()->str();
            _sessions.erase(remoteAddr);
        }

        void SocketSessionRegistry::setAllEventsHandler(
            SocketSession::allEventsHandler_t allEventsHandler)
        {
            _allEventsHandler = allEventsHandler;
        }

        void SocketSessionRegistry::setDisconnectHandler(
            SocketSession::disconnectHandler_t disconnectHandler)
        {
            _disconnectHandler = disconnectHandler;
        }

    } // namespace socket
} // namespace sevent
