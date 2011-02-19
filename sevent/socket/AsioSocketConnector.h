#pragma once
#include "SocketConnector.h"
#include "AsioSocketService.h"
#include "SocketSessionRegistry.h"
#include <boost/asio.hpp>

namespace sevent
{
namespace socket
{

class AsioSocketConnector: public SocketConnector
{
    private:
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    public:
        AsioSocketConnector(AsioSocketService_ptr socketservice,
                SocketSessionRegistry_ptr sessionRegistry);
        virtual ~AsioSocketConnector();
        virtual SocketSession_ptr connect(const Address_ptr addr);
    private:
        AsioSocketService_ptr _socketservice;
        SocketSessionRegistry_ptr _sessionRegistry;
};

} // namespace socket
} // namespace sevent
