#include "AsioResolver.h"
#include <boost/asio.hpp>

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
            return socket::Address::make(hostIp, port);
        }
    } // namespace asiosocket
} // namespace sevent
