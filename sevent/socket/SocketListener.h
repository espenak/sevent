#pragma once
#include "Address.h"

namespace sevent
{
namespace socket
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

} // namespace socket
} // namespace sevent
