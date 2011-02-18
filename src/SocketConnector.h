#pragma once
#include "SocketSession.h"
#include "Address.h"

namespace socketevent
{

class SocketConnector
{
public:
	virtual SocketSession_ptr connect(const Address_ptr addr) = 0;
};

} // namespace socketevent
