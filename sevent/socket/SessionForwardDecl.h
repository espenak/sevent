#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        class SessionRegistry;
        typedef boost::shared_ptr<SessionRegistry> SessionRegistry_ptr;
    } // namespace socket
} // namespace sevent
