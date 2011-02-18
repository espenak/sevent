#pragma once
#include "SocketSession.h"
#include <boost/shared_ptr.hpp>
#include <vector>

namespace sevent
{
namespace socket
{

class SocketSessionRegistry;
typedef boost::shared_ptr<SocketSessionRegistry> SocketSessionRegistry_ptr;

class SocketSessionRegistry
{
    public:
        static SocketSessionRegistry_ptr make();
	public:
		SocketSessionRegistry();
		virtual ~SocketSessionRegistry();
		void add(SocketSession_ptr session);
	private:
		std::vector<SocketSession_ptr> _sessions;
};

} // namespace socket
} // namespace sevent
