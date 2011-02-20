#pragma once
#include "Connector.h"
#include "AsioService.h"
#include "SessionRegistry.h"
#include <boost/asio.hpp>

namespace sevent
{
    namespace socket
    {

        class AsioConnector: public Connector
        {
            private:
                typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
            public:
                AsioConnector(AsioService_ptr socketservice,
                                    SessionRegistry_ptr sessionRegistry);
                virtual ~AsioConnector();
                virtual Session_ptr connect(const Address_ptr addr);
            private:
                AsioService_ptr _socketservice;
                SessionRegistry_ptr _sessionRegistry;
        };

    } // namespace socket
} // namespace sevent
