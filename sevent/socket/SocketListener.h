#pragma once
#include "Address.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        class SocketListener
        {
        public:
            virtual ~SocketListener()
            {
            };
            virtual void listen(Address_ptr address) = 0;
        };

        typedef boost::shared_ptr<SocketListener> SocketListener_ptr;
    } // namespace socket
} // namespace sevent
