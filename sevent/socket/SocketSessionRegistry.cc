#include "SessionRegistry.h"
#include <boost/bind.hpp>
#include <iostream>

namespace sevent
{
    namespace socket
    {

        SessionRegistry::SessionRegistry()
        {
            setDisconnectHandler(boost::bind(&SessionRegistry::remove,
                                             this, _1));
        }

        SessionRegistry::~SessionRegistry()
        {
        }

        SessionRegistry_ptr SessionRegistry::make()
        {
            return SessionRegistry_ptr(new SessionRegistry());
        }

        void SessionRegistry::add(Session_ptr session)
        {
            if (_allEventsHandler)
            {
                session->setAllEventsHandler(_allEventsHandler);
            }
            session->setDisconnectHandler(_disconnectHandler);
            std::string remoteAddr = session->getRemoteEndpointAddress()->str();
            _sessions[remoteAddr] = session;
        }

        void SessionRegistry::remove(Session_ptr session)
        {
            std::string remoteAddr = session->getRemoteEndpointAddress()->str();
            _sessions.erase(remoteAddr);
        }

        void SessionRegistry::setAllEventsHandler(
            Session::allEventsHandler_t allEventsHandler)
        {
            _allEventsHandler = allEventsHandler;
        }

        void SessionRegistry::setDisconnectHandler(
            Session::disconnectHandler_t disconnectHandler)
        {
            _disconnectHandler = disconnectHandler;
        }

    } // namespace socket
} // namespace sevent
