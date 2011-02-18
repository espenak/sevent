#pragma once

namespace socketevent
{

class SocketConnector
{
public:
	virtual SocketSession connect(const Address_ptr addr) = 0;
};

} // namespace socketevent
