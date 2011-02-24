#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        class Session;
        typedef boost::shared_ptr<Session> Session_ptr;

        class SessionRegistry;
        typedef boost::shared_ptr<SessionRegistry> SessionRegistry_ptr;
    } // namespace socket
} // namespace sevent
