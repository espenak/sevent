#include "AsioSocketConnector.h"
#include "AsioSocketSession.h"

namespace sevent
{
namespace socket
{

AsioSocketConnector::AsioSocketConnector(AsioSocketService_ptr socketservice,
        SocketSessionRegistry_ptr sessionRegistry) :
    _socketservice(socketservice), _sessionRegistry(sessionRegistry)
{
}

AsioSocketConnector::~AsioSocketConnector()
{
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
    AsioSocketSession_ptr session = AsioSocketSession::make(sock);
    _sessionRegistry->add(session);
    return session;
}

} // namespace socket
} // namespace sevent
