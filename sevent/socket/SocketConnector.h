#pragma once
#include "Session.h"
#include "Address.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {

        class Connector
        {
            public:
                virtual ~Connector()
                {
                }
                virtual Session_ptr connect(const Address_ptr addr) = 0;
        };
        typedef boost::shared_ptr<Connector> Connector_ptr;

    } // namespace socket
} // namespace sevent
