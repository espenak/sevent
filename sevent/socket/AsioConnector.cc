#include "AsioConnector.h"
#include "AsioSession.h"
#include "AsioResolver.h"

using namespace boost::asio;

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
            ip::address address = ip::address::from_string(addr->host());
            ip::tcp::endpoint endpoint(address, addr->port());

            socket_ptr sock(new boost::asio::ip::tcp::socket(
                        _socketservice->_io_service));
            sock->connect(endpoint);
            AsioSession_ptr session = AsioSession::make(sock);
            _sessionRegistry->add(session);
            return session;
        }

    } // namespace asiosocket
} // namespace sevent
