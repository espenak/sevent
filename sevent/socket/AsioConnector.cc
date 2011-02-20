#include "AsioConnector.h"
#include "AsioSession.h"
#include "AsioResolver.h"

namespace sevent
{
    namespace asiosocket
    {

        AsioConnector::AsioConnector(AsioService_ptr socketservice,
                socket::SessionRegistry_ptr sessionRegistry) :
            _socketservice(socketservice), _sessionRegistry(sessionRegistry)
        {
        }

        AsioConnector::~AsioConnector()
        {
        }

        socket::Session_ptr AsioConnector::connect(const socket::Address_ptr addr)
        {
            boost::asio::ip::tcp::resolver resolver(_socketservice->_io_service);
            boost::asio::ip::tcp::resolver::query query(addr->host(), addr->port());
            boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
            boost::asio::ip::tcp::endpoint endpoint = *iterator;

            socket_ptr sock(new boost::asio::ip::tcp::socket(
                                _socketservice->_io_service));
            sock->connect(endpoint);
            AsioSession_ptr session = AsioSession::make(sock);
            _sessionRegistry->add(session);
            return session;
        }

    } // namespace asiosocket
} // namespace sevent
