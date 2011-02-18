#pragma once
#include <string>
#include <boost/shared_ptr.hpp>

namespace socketevent
{

class Address;
typedef boost::shared_ptr<Address> Address_ptr;

class Address
{
	public:
		static Address_ptr make(const std::string& host, const std::string& port)
		{
			return Address_ptr(new Address(host, port));
		}

	public:
		Address(const std::string& host, const std::string& port) :
			_host(host), _port(port)
		{
		}

		~Address()
		{
		}

		const std::string& host();
		const std::string& port();

	private:
		std::string _host;
		std::string _port;
};

} // namespace socketevent
