#pragma once
#include <boost/shared_ptr.hpp>

namespace socketevent
{

class SocketSession
{
	public:
		SocketSession();
		virtual ~SocketSession();
};

typedef boost::shared_ptr<SocketSession> SocketSession_ptr;

}
