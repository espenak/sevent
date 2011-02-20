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
