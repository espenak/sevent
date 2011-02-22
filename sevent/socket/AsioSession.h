#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <vector>
#include "Session.h"
#include "ConstBuffer.h"
#include "MutableBuffer.h"

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
                void addToBuffers(std::vector<boost::asio::const_buffer>& buffers,
                                  const socket::ConstBuffer& const_buf);
                void onHeaderReceived(const boost::system::error_code& error,
                                         std::size_t byte_transferred);
                void receiveNextDataBuf();
                void onDataBufSizeReceived(const boost::system::error_code & error,
                                           std::size_t byte_transferred);
                void onDataBufReceived(const boost::system::error_code & error,
                                       std::size_t byte_transferred, char* data,
                                       uint32_t dataSize);
                bool handleTransferErrors(const boost::system::error_code& error,
                                          uint32_t bytesTransferred,
                                          uint32_t expectedBytesTransferred,
                                          const char* bytesTransferredErrmsg);
            private:
                socket_ptr _sock;
                boost::array<unsigned, 2> _headerBuf;
                boost::array<uint32_t, 1> _sizeBuf;
                unsigned dataBufsReceived;
                std::vector<socket::MutableBuffer> _dataBuffers;
                boost::mutex _sendLock;
                boost::mutex _receiveLock;
        };

    } // namespace asiosocket
} // namespace sevent
