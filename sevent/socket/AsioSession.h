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
                typedef boost::shared_ptr<uint32_t> uint32_ptr;
            public:
                static AsioSession_ptr make(socket_ptr sock);
            public:
                AsioSession(socket_ptr sock);
                virtual ~AsioSession();
                virtual void sendEvent(unsigned eventid, const socket::ConstBuffer& data);
                virtual void sendEvent(unsigned eventid, const socket::ConstBufferVector& dataBufs);
                virtual void receiveEvents();
                virtual socket::Address_ptr getLocalEndpointAddress();
                virtual socket::Address_ptr getRemoteEndpointAddress();
            private:
                void addToBuffers(std::vector<boost::asio::const_buffer>& buffers,
                                  const socket::ConstBuffer& const_buf);
                void onHeaderReceived(const boost::system::error_code& error,
                                      std::size_t byte_transferred);


                bool handleTransferErrors(const boost::system::error_code& error,
                                          uint32_t bytesTransferred,
                                          uint32_t expectedBytesTransferred,
                                          const char* bytesTransferredErrmsg);
            private:
                socket_ptr _sock;
                unsigned dataBufsReceived;
                std::vector<socket::MutableBuffer> _dataBuffers;
                boost::mutex _sendLock;
                boost::mutex _receiveLock;
                boost::array<uint32_t, 2> _headerBuf;
        };

    } // namespace asiosocket
} // namespace sevent
