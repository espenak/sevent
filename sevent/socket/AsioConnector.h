#pragma once
#include "Connector.h"
#include "AsioService.h"
#include "SessionRegistry.h"
#include <boost/asio.hpp>

namespace sevent
{
    namespace asiosocket
    {

        class AsioConnector: public socket::Connector
        {
            private:
                typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
            public:
                AsioConnector(AsioService_ptr socketservice,
                                    socket::SessionRegistry_ptr sessionRegistry);
                virtual ~AsioConnector();
                virtual socket::Session_ptr connect(const socket::Address_ptr addr);
            private:
                AsioService_ptr _socketservice;
                socket::SessionRegistry_ptr _sessionRegistry;
        };

    } // namespace asiosocket
} // namespace sevent
