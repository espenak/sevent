#include "AsioSession.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/bind.hpp>
#include <arpa/inet.h>

namespace sevent
{
    namespace asiosocket
    {

        AsioSession::AsioSession(socket_ptr sock) :
            _sock(sock)
        {
            setAllEventsHandler( defaultAllEventsHandler);
        }

        AsioSession::~AsioSession()
        {
        }

        AsioSession_ptr AsioSession::make(socket_ptr sock)
        {
            return AsioSession_ptr(new AsioSession(sock));
        }

        void AsioSession::sendEvent(const socket::SendEvent& event)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            uint32_t eventIdNetworkOrder = htonl(event.eventid());
            uint32_t sizeNetworkOrder = htonl(event.dataSize());

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(event.data(),
                                                  event.dataSize()));
            boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
        }

        void AsioSession::receiveEvents()
        {
            _receiveLock.lock();
            boost::asio::async_read(*_sock,
                    boost::asio::buffer(_idAndSizeBuf),
                    boost::asio::transfer_all(),
                    boost::bind(&AsioSession::onIdAndSizeReceived, this, _1, _2));
        }

        void AsioSession::onIdAndSizeReceived(
            const boost::system::error_code & error, std::size_t byte_transferred)
        {
            if (error == boost::asio::error::eof)
            {
                _disconnectHandler(shared_from_this());
                return;
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }
            else if(byte_transferred != sizeof(uint32_t)*2)
            {
                throw std::runtime_error("byte_transferred != sizeof(uint32_t)*2. This is a bug, because transfer_all() should make this impossible.");
            }
            uint32_t eventid = ntohl(_idAndSizeBuf[0]);
            uint32_t bufferSize = ntohl(_idAndSizeBuf[1]);
            char* buffer = new char[bufferSize];
            boost::asio::async_read(*_sock,
                    boost::asio::buffer(buffer, bufferSize),
                    boost::asio::transfer_all(),
                    boost::bind(&AsioSession::onDataReceived, this, _1, _2, eventid, buffer, bufferSize));
        }

        void AsioSession::onDataReceived(const boost::system::error_code & error,
                                               std::size_t byte_transferred, uint32_t eventid, char* data,
                                               uint32_t dataSize)
        {
            if (error == boost::asio::error::eof)
            {
                _disconnectHandler(shared_from_this());
                return;
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }
            else if(byte_transferred != dataSize)
            {
                throw std::runtime_error("byte_transferred != dataSize. This is a bug, because transfer_all() should make this impossible.");
            }
            socket::ReceiveEvent event(eventid, data, dataSize);
            _allEventsHandler(shared_from_this(), event);
            _receiveLock.unlock();
            receiveEvents();
        }

        socket::Address_ptr AsioSession::getRemoteEndpointAddress()
        {
            std::string host = _sock->remote_endpoint().address().to_string();
            unsigned short port = _sock->remote_endpoint().port();
            return socket::Address::make(host, port);
        }

        socket::Address_ptr AsioSession::getLocalEndpointAddress()
        {
            std::string host = _sock->local_endpoint().address().to_string();
            unsigned short port = _sock->local_endpoint().port();
            return socket::Address::make(host, port);
        }

    } // namespace asiosocket
} // namespace sevent
