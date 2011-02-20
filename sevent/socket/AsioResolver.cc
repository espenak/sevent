#include "AsioResolver.h"
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

namespace sevent {
    namespace asiosocket {
        socket::Address_ptr resolveToAddress(AsioService_ptr socketservice,
                const std::string& host, unsigned short port)
        {
            boost::asio::ip::tcp::resolver resolver(socketservice->_io_service);
            boost::asio::ip::tcp::resolver::query query(host);
            boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
            boost::asio::ip::tcp::endpoint endpoint = *iterator;

            std::string hostIp = endpoint.address().to_string();
            //unsigned short port = _sock->remote_endpoint().port();
            return socket::Address::make(hostIp, boost::lexical_cast<std::string>(port));
        }
    } // namespace asiosocket
} // namespace sevent
