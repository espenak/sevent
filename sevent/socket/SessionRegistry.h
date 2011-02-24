#pragma once
#include "SessionForwardDecl.h"
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
                void setDisconnectHandler(Session::disconnectHandler_t disconnectHandler);
                void setAllEventsHandler(Session::allEventsHandler_t allEventsHandler);

            private:
                boost::unordered_map<std::string, Session_ptr> _sessions;
                Session::allEventsHandler_t _allEventsHandler;
                Session::disconnectHandler_t _disconnectHandler;
                boost::mutex _lock;
        };

    } // namespace socket
} // namespace sevent
