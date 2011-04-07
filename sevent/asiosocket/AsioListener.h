#pragma once
#include "sevent/socket/Listener.h"
#include "sevent/socket/Address.h"
#include "sevent/socket/SessionRegistry.h"
#include "AsioService.h"
#include <boost/asio.hpp>

namespace sevent
{
    namespace asiosocket
    {

        class AsioListener: public socket::Listener
        {
            private:
                typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
            public:
                AsioListener(AsioService_ptr socketservice,
                                   socket::SessionRegistry_ptr socketSessionRegistry);
                virtual ~AsioListener();
                virtual void listen(socket::Address_ptr address);
            private:
                void accept();
                void onAccept(const boost::system::error_code & ec, socket_ptr sock);
            private:
                AsioService_ptr _socketservice;
                boost::asio::ip::tcp::acceptor _acceptor;
                socket::SessionRegistry_ptr _socketSessionRegistry;
        };

    } // namespace asiosocket
} // namespace sevent
