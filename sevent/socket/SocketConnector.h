#pragma once
#include "SocketSession.h"
#include "Address.h"

namespace sevent
{
namespace socket
{

class SocketConnector
{
    public:
        virtual ~SocketConnector()
        {
        }
        virtual SocketSession_ptr connect(const Address_ptr addr) = 0;
};

} // namespace socket
} // namespace sevent