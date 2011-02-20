#include "AsioSession.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <arpa/inet.h>

namespace sevent
{
    namespace socket
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

        void AsioSession::sendEvent(EventData eventData)
        {
            uint32_t eventIdNetworkOrder = htonl(eventData.eventid());
            uint32_t sizeNetworkOrder = htonl(eventData.dataSize());

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(eventData.data(),
                                                  eventData.dataSize()));
            boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
        }

        void AsioSession::receiveEvents()
        {
            _sock->async_receive(boost::asio::buffer(_idAndSizeBuf), boost::bind(
                                     &AsioSession::onIdAndSizeReceived, this, _1, _2));
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
            uint32_t eventid = ntohl(_idAndSizeBuf[0]);
            uint32_t bufferSize = ntohl(_idAndSizeBuf[1]);
            char* buffer = new char[bufferSize];
            _sock->async_receive(boost::asio::buffer(buffer, bufferSize), boost::bind(
                                     &AsioSession::onDataReceived, this, _1, _2, eventid, buffer,
                                     bufferSize));
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
            _allEventsHandler(shared_from_this(), EventData(eventid, data, dataSize));
            receiveEvents();
        }

        Address_ptr AsioSession::getRemoteEndpointAddress()
        {
            std::string host = _sock->remote_endpoint().address().to_string();
            unsigned short port = _sock->remote_endpoint().port();
            return Address::make(host, boost::lexical_cast<std::string>(port));
        }

        Address_ptr AsioSession::getLocalEndpointAddress()
        {
            std::string host = _sock->local_endpoint().address().to_string();
            unsigned short port = _sock->local_endpoint().port();
            return Address::make(host, boost::lexical_cast<std::string>(port));
        }

    } // namespace socket
} // namespace sevent
