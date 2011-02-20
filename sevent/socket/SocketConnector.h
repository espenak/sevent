#pragma once
#include "SocketSession.h"
#include "Address.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {

        class SocketConnector
        {
            public:
                virtual ~SocketConnector()
                {
                }
                virtual SocketSession_ptr connect(const Address_ptr addr) = 0;
        };
        typedef boost::shared_ptr<SocketConnector> SocketConnector_ptr;

    } // namespace socket
} // namespace sevent
