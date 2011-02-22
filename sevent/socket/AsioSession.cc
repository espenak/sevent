#include "AsioSession.h"
#include <iostream>
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


        void AsioSession::addToBuffers(std::vector<boost::asio::const_buffer>& buffers,
                                       const socket::ConstBuffer& const_buf)
        {
            uint32_t sizeNetworkOrder = htonl(const_buf.size());
            buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(const_buf.data(),
                                                  const_buf.size()));
        }

        void AsioSession::sendEvent(const socket::SendEvent& event)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            uint32_t eventIdNetworkOrder = htonl(event.eventid());
            uint32_t numElementsNetworkOrder = htonl(1);

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(&numElementsNetworkOrder,
                                                  sizeof(uint32_t)));
            //buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
            //buffers.push_back(boost::asio::buffer(event.data(),
                                                  //event.dataSize()));
            addToBuffers(buffers, socket::ConstBuffer(event.data(), event.dataSize()));
            boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
        }

        void AsioSession::receiveEvents()
        {
            _receiveLock.lock();
            dataBufsReceived = 0;
            boost::asio::async_read(*_sock,
                    boost::asio::buffer(_headerBuf),
                    boost::asio::transfer_all(),
                    boost::bind(&AsioSession::onHeaderReceived, this, _1, _2));
        }


        void AsioSession::onHeaderReceived(
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
            receiveNextDataBuf();
        }

        void AsioSession::receiveNextDataBuf()
        {
            uint32_t numElements = ntohl(_headerBuf[1]);
            if(dataBufsReceived < numElements)
            {
                boost::asio::async_read(*_sock,
                                        boost::asio::buffer(_sizeBuf),
                                        boost::asio::transfer_all(),
                                        boost::bind(&AsioSession::onDataBufSizeReceived, this, _1, _2));
            } else {
                uint32_t eventid = ntohl(_headerBuf[0]);
                socket::ReceiveEvent event(eventid, (char*) _dataBuffers[0].data(), _dataBuffers[0].size());
                _allEventsHandler(shared_from_this(), event);
                _dataBuffers.clear();

                _receiveLock.unlock();
                receiveEvents();
            }
        }

        void AsioSession::onDataBufSizeReceived(const boost::system::error_code & error,
                                                std::size_t byte_transferred)
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
            else if(byte_transferred != sizeof(uint32_t))
            {
                throw std::runtime_error("byte_transferred != sizeof(uint32_t). This is a bug, because transfer_all() should make this impossible.");
            }
            uint32_t dataSize = ntohl(_sizeBuf[0]);
            char* data = new char[dataSize];
            boost::asio::async_read(*_sock,
                    boost::asio::buffer(data, dataSize),
                    boost::asio::transfer_all(),
                    boost::bind(&AsioSession::onDataBufReceived, this,
                                _1, _2, data, dataSize));
        }

        void AsioSession::onDataBufReceived(const boost::system::error_code & error,
                                            std::size_t byte_transferred, char* data,
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
            _dataBuffers.push_back(socket::MutableBuffer(data, dataSize));
            dataBufsReceived ++;
            receiveNextDataBuf();
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
