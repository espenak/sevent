#pragma once
#include <string>
#include <boost/shared_ptr.hpp>
#include <ostream>

namespace sevent
{
    namespace socket
    {

        class Address;
        typedef boost::shared_ptr<Address> Address_ptr;

        /** A pair of (host, port).
         *
         * The host is not a hostname, but a resolved (ip) address.
         *
         * It is important that the host+port is a unambiguous and unique address
         * within a network using an implemenation of sevent::socket, since this
         * is the key used to check for existing connections.
         *
         * The host is a string to make it possible to use any
         * kind of address. The asio implementation uses this for both ipv4
         * (dot notation) and ipv6 (hex encoding).
         */
        class Address
        {
            public:
                static Address_ptr make(const std::string& host,
                                        unsigned short port);

            public:
                Address(const std::string& host, unsigned short port);
                virtual ~Address();
                const std::string& host() const;
                unsigned short port() const;
                std::string str() const;

            private:
                std::string _host;
                unsigned short _port;
        };

        std::ostream& operator<<(std::ostream& out, Address address);
        std::ostream& operator<<(std::ostream& out, Address_ptr address);

    } // namespace socket
} // namespace sevent
