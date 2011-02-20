#include "Address.h"
#include <boost/lexical_cast.hpp>

namespace sevent
{
    namespace socket
    {

        Address::Address(const std::string& host, unsigned short port) :
            _host(host), _port(port)
        {
        }

        Address::~Address()
        {
        }

        Address_ptr Address::make(const std::string& host, unsigned short port)
        {
            return Address_ptr(new Address(host, port));
        }

        const std::string& Address::host() const
        {
            return _host;
        }
        unsigned short Address::port() const
        {
            return _port;
        }

        std::string Address::str() const
        {
            return _host + ":" + boost::lexical_cast<std::string>(_port);
        }

        std::ostream& operator<<(std::ostream& out, const Address& address)
        {
            out << address.host() << ":" << address.port();
            return out;
        }

        std::ostream& operator<<(std::ostream& out, const Address_ptr address)
        {
            out << address->host() << ":" << address->port();
            return out;
        }


    } // namespace socket
} // namespace sevent
