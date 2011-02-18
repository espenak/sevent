#pragma once
#include "SocketConnector.h"
#include "AsioSocketService.h"
#include <boost/asio.hpp>

namespace socketevent
{

class AsioSocketConnector: public SocketConnector
{
    private:
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    public:
        AsioSocketConnector(AsioSocketService_ptr socketservice);
        virtual ~AsioSocketConnector();
        virtual SocketSession_ptr connect(const Address_ptr addr);
    private:
        void connect();
        void onConnect(const boost::system::error_code & ec, socket_ptr sock);
    private:
        AsioSocketService_ptr _socketservice;
};

}
