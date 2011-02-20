#include "AsioListener.h"
#include <boost/bind.hpp>
#include "AsioSession.h"

using namespace boost::asio;

namespace sevent
{
    namespace asiosocket
    {

        AsioListener::AsioListener(AsioService_ptr socketservice,
                socket::SessionRegistry_ptr socketSessionRegistry) :
            _socketservice(socketservice), _acceptor(socketservice->_io_service),
            _socketSessionRegistry(socketSessionRegistry)
        {
        }

        AsioListener::~AsioListener()
        {
        }

        void AsioListener::listen(socket::Address_ptr addr)
        {
            ip::address address = ip::address::from_string(addr->host());
            ip::tcp::endpoint endpoint(address, addr->port());

            _acceptor.open(endpoint.protocol());
            _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
            _acceptor.bind(endpoint);
            _acceptor.listen(boost::asio::socket_base::max_connections);
            accept();
        }

        void AsioListener::accept()
        {
            socket_ptr sock(new boost::asio::ip::tcp::socket(
                                _socketservice->_io_service));
            _acceptor.async_accept(*sock, boost::bind(&AsioListener::onAccept,
                                   this, _1, sock));
        }

        void AsioListener::onAccept(const boost::system::error_code & ec,
                                          socket_ptr sock)
        {
            AsioSession_ptr session = AsioSession::make(sock);
            _socketSessionRegistry->add(session);
            session->receiveEvents();
            accept();
        }

    } // namespace asiosocket
} // namespace sevent
