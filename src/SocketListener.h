#pragma once
#include "Address.h"

namespace socketevent
{

class SocketListener
{
	public:
		virtual ~SocketListener()
        {
        }
        ;
		virtual void listen(Address_ptr address) = 0;
};

}
