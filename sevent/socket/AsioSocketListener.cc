#include "AsioSocketListener.h"
#include <boost/bind.hpp>
#include "AsioSocketSession.h"

namespace sevent
{
    namespace socket
    {

        AsioSocketListener::AsioSocketListener(AsioSocketService_ptr socketservice,
                                               SocketSessionRegistry_ptr socketSessionRegistry) :
            _socketservice(socketservice), _acceptor(socketservice->_io_service),
            _socketSessionRegistry(socketSessionRegistry)
        {
        }

        AsioSocketListener::~AsioSocketListener()
        {
        }

        void AsioSocketListener::listen(Address_ptr address)
        {
            boost::asio::ip::tcp::resolver resolver(_socketservice->_io_service);
            boost::asio::ip::tcp::resolver::query query(address->host(),
                    address->port());
            boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen(boost::asio::socket_base::max_connections);
            accept();
        }

        void AsioSocketListener::accept()
        {
            socket_ptr sock(new boost::asio::ip::tcp::socket(
                                _socketservice->_io_service));
            _acceptor.async_accept(*sock, boost::bind(&AsioSocketListener::onAccept,
                                   this, _1, sock));
        }

        void AsioSocketListener::onAccept(const boost::system::error_code & ec,
                                          socket_ptr sock)
        {
            AsioSocketSession_ptr session = AsioSocketSession::make(sock);
            _socketSessionRegistry->add(session);
            session->receiveEvents();
            accept();
        }

    } // namespace socket
} // namespace sevent
