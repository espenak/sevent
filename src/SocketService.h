#pragma once

namespace socketevent
{

class SocketService
{
	public:
		SocketService();
		virtual ~SocketService();
		virtual void run() = 0;
};

}
