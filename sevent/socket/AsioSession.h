#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include "Session.h"

namespace sevent
{
    namespace asiosocket
    {

        class AsioSession;
        typedef boost::shared_ptr<AsioSession> AsioSession_ptr;

        class AsioSession: public socket::Session,
            public boost::enable_shared_from_this<AsioSession>
        {
            private:
                typedef boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
            public:
                static AsioSession_ptr make(socket_ptr sock);
            public:
                AsioSession(socket_ptr sock);
                virtual ~AsioSession();
                virtual void sendEvent(const socket::SendEvent& event);
                virtual void receiveEvents();
                virtual socket::Address_ptr getLocalEndpointAddress();
                virtual socket::Address_ptr getRemoteEndpointAddress();
            private:
                void onIdAndSizeReceived(const boost::system::error_code& error,
                                         std::size_t byte_transferred);
                void onDataReceived(const boost::system::error_code& error,
                                    std::size_t byte_transferred, uint32_t eventid, char* data,
                                    uint32_t dataSize);
            private:
                socket_ptr _sock;
                boost::array<unsigned, 2> _idAndSizeBuf;
                boost::mutex _receiveLock;
                boost::mutex _sendLock;
        };

    } // namespace asiosocket
} // namespace sevent
