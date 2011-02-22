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
            //std::cerr << "Adding " << const_buf.size();
            //std::cerr.write((const char*) const_buf.data(), const_buf.size());
            //std::cerr << std::endl;
            uint32_t sizeNetworkOrder = htonl(const_buf.size());
            buffers.push_back(boost::asio::buffer(&sizeNetworkOrder, sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(const_buf.data(),
                                                  const_buf.size()));
        }

        void AsioSession::sendEvent(unsigned eventid, const socket::ConstBuffer& data)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            uint32_t eventIdNetworkOrder = htonl(eventid);
            uint32_t numElementsNetworkOrder = htonl(1);

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(&numElementsNetworkOrder,
                                                  sizeof(uint32_t)));
            addToBuffers(buffers, data);
            boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
        }

        void AsioSession::sendEvent(unsigned eventid, const socket::ConstBufferVector& dataBufs)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            uint32_t eventIdNetworkOrder = htonl(eventid);
            uint32_t numElementsNetworkOrder = htonl(dataBufs.size());

            std::vector<boost::asio::const_buffer> buffers;
            buffers.push_back(boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)));
            buffers.push_back(boost::asio::buffer(&numElementsNetworkOrder,
                                                  sizeof(uint32_t)));
            for(int i = 0; i < dataBufs.size(); i++)
            {
                addToBuffers(buffers, dataBufs.at(i));
            }
            //addToBuffers(buffers, dataBufs.at(0));
            boost::asio::write(*_sock, buffers, boost::asio::transfer_all());
        }

        void AsioSession::receiveEvents()
        {
            _receiveLock.lock();
            std::cerr << "receiveEvents" << std::endl;
            boost::asio::async_read(*_sock,
                                    boost::asio::buffer(headerBuf),
                                    boost::asio::transfer_all(),
                                    boost::bind(&AsioSession::onHeaderReceived,
                                                this, _1, _2));
        }


        void AsioSession::onHeaderReceived(const boost::system::error_code & error,
                                           std::size_t byte_transferred)
        {
            if(!handleTransferErrors(error,
                                     byte_transferred,
                                     sizeof(uint32_t)*2,
                                     "byte_transferred != sizeof(uint32_t)*2"))
            {
                return;
            }
            uint32_t eventid = ntohl(headerBuf[0]);
            uint32_t numElements = ntohl(headerBuf[1]);

            std::cerr << "onHeaderReceived "
                << " eventid:" << eventid
                << " elemcount:" << numElements
                << std::endl;

            boost::array<uint32_t, 1> dataSizeBuf;
            boost::asio::read(*_sock, boost::asio::buffer(dataSizeBuf), boost::asio::transfer_all());
            uint32_t dataSize = ntohl(dataSizeBuf[0]);

            char* data = new char[dataSize];
            boost::asio::read(*_sock,
                              boost::asio::buffer(data, dataSize),
                              boost::asio::transfer_all());

            socket::ReceiveEvent event(eventid,
                                       data,
                                       dataSize);
            _allEventsHandler(shared_from_this(), event);
            std::cerr << "data: " << data << std::endl;
            _receiveLock.unlock();
            receiveEvents();
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


        /*
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
            std::cerr << "onHeaderReceived "
                << " eventid:" << ntohl(_headerBuf[0])
                << " elemcount:" << ntohl(_headerBuf[1])
                << std::endl;
            receiveNextDataBuf();
        }

        void AsioSession::receiveNextDataBuf()
        {
            std::cerr << "receiveNextDataBuf" << std::endl;
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
            std::cerr << "triggerAllEventsHandler" << std::endl;
            uint32_t eventid = ntohl(_headerBuf[0]);
            socket::ReceiveEvent event(eventid, (char*) _dataBuffers[0].data(), _dataBuffers[0].size());
            _allEventsHandler(shared_from_this(), event);
            _dataBuffers.clear();
        }

        void AsioSession::receiveDataBufSize()
        {
            std::cerr << "receiveDataBufSize" << std::endl;
            uint32_t* sizeBuf = new uint32_t[1];
            boost::asio::async_read(*_sock,
                                    boost::asio::buffer((char*)sizeBuf, sizeof(uint32_t)),
                                    boost::asio::transfer_all(),
                                    boost::bind(&AsioSession::onDataBufSizeReceived,
                                                this, _1, _2, sizeBuf));
        }

        void AsioSession::onDataBufSizeReceived(const boost::system::error_code & error,
                                                std::size_t byte_transferred,
                                                uint32_t* sizeBuf)
        {
            if(!handleTransferErrors(error,
                                     byte_transferred,
                                     sizeof(uint32_t),
                                     "byte_transferred != sizeof(uint32_t)"))
            {
                return;
            }
            uint32_t dataSize = ntohl(*sizeBuf);
            std::cerr << "onDataBufSizeReceived: " << dataSize << std::endl;
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
            std::cerr << "onDataBufReceived: "
                << "received:" << dataBufsReceived
                << " data:" << data
                << " datasize:" << dataSize
                << std::endl;
            receiveNextDataBuf();
        }
        */

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
