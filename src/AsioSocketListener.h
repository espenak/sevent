#pragma once
#include "SocketListener.h"
#include "Address.h"
#include "AsioSocketService.h"
#include <boost/asio.hpp>

namespace socketevent
{

class AsioSocketListener: public SocketListener
{
    private:
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    public:
        AsioSocketListener(AsioSocketService_ptr socketservice);
        virtual ~AsioSocketListener();
        virtual void listen(Address_ptr address);
    private:
        void accept();
        void onAccept(const boost::system::error_code & ec, socket_ptr sock);
    private:
        AsioSocketService_ptr _socketservice;
        boost::asio::ip::tcp::acceptor _acceptor;
};

}
