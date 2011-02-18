#include "AsioSocketSession.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <boost/bind.hpp>
#include <arpa/inet.h>

namespace socketevent
{

AsioSocketSession::AsioSocketSession(socket_ptr sock) :
    _sock(sock)
{
}

AsioSocketSession::~AsioSocketSession()
{
}

AsioSocketSession_ptr AsioSocketSession::make(socket_ptr sock)
{
    return AsioSocketSession_ptr(new AsioSocketSession(sock));
}

void AsioSocketSession::sendEvent(eventId_t eventid, Buffer buffer) // TODO: Do we need Buffer? maybe just char*, size..
{
    uint32_t eventIdNetworkOrder = htonl(eventid);
    uint32_t sizeNetworkOrder = htonl(buffer.size());

    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
            sizeof(uint32_t)));
    buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
    buffers.push_back(boost::asio::buffer(buffer.data(), buffer.size()));
    boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
}

//void AsioSocketSession::asyncSendEvent(eventId_t eventid, Buffer buffer, SendEventCallback_t)
//{
//    boost::asio::async_write(_sock,
//            boost::asio::buffer(boost::array<char, buffer.size()>(buffer.data()),
//                    boost::bind(callback, _1, _2))));
//}

void AsioSocketSession::reveiveEvents()
{
    _sock->async_receive(boost::asio::buffer(_idAndSizeBuf), boost::bind(
            &AsioSocketSession::onIdAndSizeReceived, this, _1, _2));
}

void AsioSocketSession::onIdAndSizeReceived(
        const boost::system::error_code & ec, std::size_t byte_transferred)
{
    // TODO: lock while waiting for async ops to complete.
    uint32_t eventid = ntohl(_idAndSizeBuf[0]);
    uint32_t bufferSize = ntohl(_idAndSizeBuf[1]);
    std::cout << "Received event: " << "id: " << eventid << " bufferSize: "
            << bufferSize << std::endl;
    char* buffer = new char[bufferSize];
    _sock->async_receive(boost::asio::buffer(buffer, bufferSize), boost::bind(
            &AsioSocketSession::onDataReceived, this, _1, _2,
            eventid, buffer, bufferSize));
}

void AsioSocketSession::onDataReceived(const boost::system::error_code & ec,
        std::size_t byte_transferred, uint32_t eventid, char* buffer,
        uint32_t bufferSize)
{
    std::cout << "Received data: ";
    std::cout.write(buffer, bufferSize);
    std::cout << std::endl;
}

} // namespace socketevent
