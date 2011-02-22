#include "AsioSession.h"
#include <iostream>
#include <stdint.h>
#include <boost/bind.hpp>
#include <arpa/inet.h>
#include <string>

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

            //const socket::ConstBufferVector_ptr b = event.buffers();
            //for(int i = 0; i < event.buffers()->size(); i++)
            //{
                //addToBuffers(buffers, event.buffers()->at(i));
            //}
            //addToBuffers(buffers, event.buffers()->at(0));
            addToBuffers(buffers, event.at(0));
            //std::cerr
                //<< event.buffers()->at(0).size()
                //<< std::string((char*) event.buffers()->at(0).data())
                //<< std::endl;
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


        bool AsioSession::handleTransferErrors(const boost::system::error_code& error,
                                               uint32_t bytesTransferred,
                                               uint32_t expectedBytesTransferred,
                                               const char* bytesTransferredErrmsg)
        {
            if (error == boost::asio::error::eof)
            {
                _disconnectHandler(shared_from_this());
                return false;
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }
            else if(bytesTransferred != expectedBytesTransferred)
            {
                throw std::runtime_error(bytesTransferredErrmsg); // This is a bug, because transfer_all() should make this impossible.
            }
            return true;
        }


        void AsioSession::onHeaderReceived(
            const boost::system::error_code & error, std::size_t byte_transferred)
        {
            if(!handleTransferErrors(error,
                                     byte_transferred,
                                     sizeof(uint32_t)*2,
                                     "byte_transferred != sizeof(uint32_t)*2"))
            {
                return;
            }
            receiveNextDataBuf();
        }

        void AsioSession::receiveNextDataBuf()
        {
            uint32_t numElements = ntohl(_headerBuf[1]);
            if(dataBufsReceived < numElements)
            {
                receiveDataBufSize();
            } else {
                triggerAllEventsHandler();
                _receiveLock.unlock();
                receiveEvents();
            }
        }

        void AsioSession::triggerAllEventsHandler()
        {
            uint32_t eventid = ntohl(_headerBuf[0]);
            socket::ReceiveEvent event(eventid, (char*) _dataBuffers[0].data(), _dataBuffers[0].size());
            _allEventsHandler(shared_from_this(), event);
            _dataBuffers.clear();
        }

        void AsioSession::receiveDataBufSize()
        {
            boost::asio::async_read(*_sock,
                                    boost::asio::buffer(_sizeBuf),
                                    boost::asio::transfer_all(),
                                    boost::bind(&AsioSession::onDataBufSizeReceived, this, _1, _2));
        }

        void AsioSession::onDataBufSizeReceived(const boost::system::error_code & error,
                                                std::size_t byte_transferred)
        {
            if(!handleTransferErrors(error,
                                     byte_transferred,
                                     sizeof(uint32_t),
                                     "byte_transferred != sizeof(uint32_t)"))
            {
                return;
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
            if(!handleTransferErrors(error,
                                     byte_transferred,
                                     dataSize,
                                     "byte_transferred != dataSize"))
            {
                return;
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
