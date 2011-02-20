#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {

        class SocketService
        {
            public:
                virtual ~SocketService()
                {
                }
                virtual void run() = 0;
                virtual void stop() = 0;
        };
        typedef boost::shared_ptr<SocketService> SocketService_ptr;

    } // namespace socket
} // namespace sevent
