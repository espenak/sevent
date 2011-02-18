#include "AsioSocketListener.h"
#include <iostream>
#include <boost/bind.hpp>

namespace socketevent
{

AsioSocketListener::AsioSocketListener(AsioSocketService_ptr socketservice) :
    _socketservice(socketservice), _acceptor(socketservice->_io_service)
{

}

AsioSocketListener::~AsioSocketListener()
{
    // TODO Auto-generated destructor stub
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
    socket_ptr sock(new boost::asio::ip::tcp::socket( _socketservice->_io_service ));
    _acceptor.async_accept( *sock, boost::bind( &AsioSocketListener::onAccept, this, _1, sock ) );
}

void AsioSocketListener::onAccept(const boost::system::error_code & ec,
        socket_ptr sock)
{
    std::cout << "Socket connection accepted" << std::endl;
    accept();
}

}
