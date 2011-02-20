#include "AsioConnector.h"
#include "AsioSession.h"

namespace sevent
{
    namespace socket
    {

        AsioConnector::AsioConnector(AsioService_ptr socketservice,
                SessionRegistry_ptr sessionRegistry) :
            _socketservice(socketservice), _sessionRegistry(sessionRegistry)
        {
        }

        AsioConnector::~AsioConnector()
        {
        }

        Session_ptr AsioConnector::connect(const Address_ptr addr)
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

    } // namespace socket
} // namespace sevent
