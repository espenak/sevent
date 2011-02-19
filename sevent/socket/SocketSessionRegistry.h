#pragma once
#include "SocketSession.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
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
        static SocketSessionRegistry_ptr make(
                SocketSession::allEventsHandler_t allEventsHandler);
    public:
        SocketSessionRegistry();
        SocketSessionRegistry(
                SocketSession::allEventsHandler_t allEventsHandler);
        virtual ~SocketSessionRegistry();
        void add(SocketSession_ptr session);
        void remove(SocketSession_ptr session);
    private:
        boost::unordered_map<std::string, SocketSession_ptr> _sessions;
        SocketSession::allEventsHandler_t _allEventsHandler;
        SocketSession::disconnectHandler_t _disconnectHandler;
};

} // namespace socket
} // namespace sevent
