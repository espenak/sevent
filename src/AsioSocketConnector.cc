#include "AsioSocketConnector.h"
#include "AsioSocketSession.h"

namespace socketevent
{

AsioSocketConnector::AsioSocketConnector(AsioSocketService_ptr socketservice) :
    _socketservice(socketservice)
{
    // TODO Auto-generated constructor stub

}

AsioSocketConnector::~AsioSocketConnector()
{
    // TODO Auto-generated destructor stub
}

SocketSession_ptr AsioSocketConnector::connect(const Address_ptr addr)
{
    boost::asio::ip::tcp::resolver resolver(_socketservice->_io_service);
    boost::asio::ip::tcp::resolver::query query(addr->host(), addr->port());
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *iterator;

    socket_ptr sock(new boost::asio::ip::tcp::socket(
            _socketservice->_io_service));
    sock->connect(endpoint);
    return AsioSocketSession::make(sock);

    //    sock->async_connect(endpoint, boost::bind(&AsioSocketConnector::onConnect,
    //            this, _1, sock));
}

//void AsioSocketConnector::onConnect(const boost::system::error_code & ec,
//        socket_ptr sock)
//{
//}

}
