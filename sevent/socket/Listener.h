#pragma once
#include "Address.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        class Listener
        {
            public:
                virtual ~Listener()
                {
                };
                virtual void listen(Address_ptr address) = 0;
        };

        typedef boost::shared_ptr<Listener> Listener_ptr;
    } // namespace socket
} // namespace sevent
