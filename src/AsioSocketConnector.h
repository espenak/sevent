#pragma once

namespace socketevent
{

class AsioSocketConnector : public SocketConnector
{
public:
	AsioSocketConnector();
	virtual ~AsioSocketConnector();
};

}
