#pragma once

#include "SocketService.h"

namespace socketevent
{

class AsioSocketService : public SocketService
{
	public:
		AsioSocketService();
		virtual ~AsioSocketService();
		virtual void run();
};

}
