#pragma once
#include "SessionRegistryForwardDecl.h"
#include "Session.h"
#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <vector>

namespace sevent
{
    namespace socket
    {
        class SessionRegistry
        {
            public:
                static SessionRegistry_ptr make();
            public:
                SessionRegistry();
                virtual ~SessionRegistry();
                void add(Session_ptr session);
                void remove(Session_ptr session);

                /** Set the disconnect handler. This is forwarded to
                 * Session::setDisconnectHandler() when a new session is added.
                 *
                 * The default handler removes the session from the registry.
                 * Custom handlers MUST remember to do the same. */
                void setDisconnectHandler(Session::disconnectHandler_t disconnectHandler);

                /** Set the all-events-handler. This is forwarded to
                 * Session::setAllEventsHandler() when a new session is added.
                 *
                 * The default is to keep Session::defaultAllEventsHandler(). */
                void setAllEventsHandler(Session::allEventsHandler_t allEventsHandler);

            private:
                boost::unordered_map<std::string, Session_ptr> _sessions;
                Session::allEventsHandler_t _allEventsHandler;
                Session::disconnectHandler_t _disconnectHandler;
                boost::mutex _lock;
        };

    } // namespace socket
} // namespace sevent
