#include "Address.h"
#include <boost/lexical_cast.hpp>

namespace sevent
{
    namespace socket
    {
        Address::Address()
        {}

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

        std::size_t hash_value(Address const& a)
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, a._host);
            boost::hash_combine(seed, a._port);

            return seed;
        }

        bool operator==(Address const& a1, Address const& a2)
        {
            return ( a1._host == a2._host ) && ( a1._port == a2._port );
        }

        bool operator<(Address const& a1, Address const& a2)
        {
            std::string s1( a1._host + boost::lexical_cast<std::string>(a1._port));
            std::string s2( a2._host + boost::lexical_cast<std::string>(a2._port));
            return ( s1 < s2 );
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
