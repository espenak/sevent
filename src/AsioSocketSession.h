#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "SocketSession.h"

namespace socketevent
{

class AsioSocketSession;
typedef boost::shared_ptr<AsioSocketSession> AsioSocketSession_ptr;

class AsioSocketSession: public SocketSession
{
    private:
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    public:
        static AsioSocketSession_ptr make(socket_ptr sock);
    public:
        AsioSocketSession(socket_ptr sock);
        virtual ~AsioSocketSession();
        virtual void sendEvent(EventData eventData);
        virtual void reveiveEvents();
    private:
        void onIdAndSizeReceived(const boost::system::error_code & ec,
                std::size_t byte_transferred);
        void onDataReceived(const boost::system::error_code & ec,
                std::size_t byte_transferred, uint32_t eventid, char* data,
                uint32_t dataSize);
    private:
        socket_ptr _sock;
        boost::array<unsigned, 2> _idAndSizeBuf;
};

} // namespace socketevent
