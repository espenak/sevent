#pragma once
#include "SocketConnector.h"

namespace socketevent
{

class AsioSocketConnector: public SocketConnector
{
	public:
		AsioSocketConnector();
		virtual ~AsioSocketConnector();
		virtual SocketSession_ptr connect(const Address_ptr addr);
};

}
