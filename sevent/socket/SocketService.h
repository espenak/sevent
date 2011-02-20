#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {

        class Service
        {
            public:
                virtual ~Service()
                {
                }
                virtual void run() = 0;
                virtual void stop() = 0;
        };
        typedef boost::shared_ptr<Service> Service_ptr;

    } // namespace socket
} // namespace sevent
