#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include "SocketSession.h"

namespace sevent
{
namespace socket
{

class AsioSocketSession;
typedef boost::shared_ptr<AsioSocketSession> AsioSocketSession_ptr;

class AsioSocketSession: public SocketSession,
        public boost::enable_shared_from_this<AsioSocketSession>
{
    private:
        typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    public:
        static AsioSocketSession_ptr make(socket_ptr sock);
    public:
        AsioSocketSession(socket_ptr sock);
        virtual ~AsioSocketSession();
        virtual void sendEvent(EventData eventData);
        virtual void receiveEvents();
        virtual std::string getLocalEndpointInfo();
        virtual std::string getRemoteEndpointInfo();
    private:
        void onIdAndSizeReceived(const boost::system::error_code& error,
                std::size_t byte_transferred);
        void onDataReceived(const boost::system::error_code& error,
                std::size_t byte_transferred, uint32_t eventid, char* data,
                uint32_t dataSize);
    private:
        socket_ptr _sock;
        boost::array<unsigned, 2> _idAndSizeBuf;
};

} // namespace socket
} // namespace sevent
