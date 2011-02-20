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
                                        const std::string& port);

            public:
                Address(const std::string& host, const std::string& port);
                virtual ~Address();
                const std::string& host() const;
                const std::string& port() const;
                std::string str() const;

            private:
                std::string _host;
                std::string _port;
        };

        std::ostream& operator<<(std::ostream& out, Address address);
        std::ostream& operator<<(std::ostream& out, Address_ptr address);

    } // namespace socket
} // namespace sevent
