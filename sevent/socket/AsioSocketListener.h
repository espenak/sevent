#pragma once
#include "Listener.h"
#include "Address.h"
#include "AsioService.h"
#include "SessionRegistry.h"
#include <boost/asio.hpp>

namespace sevent
{
    namespace socket
    {

        class AsioListener: public Listener
        {
            private:
                typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
            public:
                AsioListener(AsioService_ptr socketservice,
                                   SessionRegistry_ptr socketSessionRegistry);
                virtual ~AsioListener();
                virtual void listen(Address_ptr address);
            private:
                void accept();
                void onAccept(const boost::system::error_code & ec, socket_ptr sock);
            private:
                AsioService_ptr _socketservice;
                boost::asio::ip::tcp::acceptor _acceptor;
                SessionRegistry_ptr _socketSessionRegistry;
        };

    } // namespace socket
} // namespace sevent
