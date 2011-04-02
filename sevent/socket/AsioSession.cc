#include "AsioSession.h"
#include <iostream>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/foreach.hpp>
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


        void AsioSession::sendHeader(unsigned eventid, int numElements)
        {
            uint32_t eventIdNetworkOrder = htonl(eventid);
            uint32_t numElementsNetworkOrder = htonl(numElements);
            boost::asio::write(*_sock,
                               boost::asio::buffer(&eventIdNetworkOrder,
                                                  sizeof(uint32_t)),
                               boost::asio::transfer_all());
            boost::asio::write(*_sock,
                               boost::asio::buffer(&numElementsNetworkOrder,
                                                  sizeof(uint32_t)),
                               boost::asio::transfer_all());
        }

        void AsioSession::sendData(const socket::Serialized& data)
        {
            uint32_t sizeNetworkOrder = htonl(data.size());
            const char* s = (const char*) data.data();
            boost::asio::write(*_sock,
                               boost::asio::buffer(&sizeNetworkOrder,
                                                   sizeof(uint32_t)),
                               boost::asio::transfer_all());
            boost::asio::write(*_sock,
                               boost::asio::buffer(data.data(), data.size()),
                               boost::asio::transfer_all());
        }

        void AsioSession::sendEvent(unsigned eventid)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            sendHeader(eventid, 0);
            //std::cerr << "Sent data: " << static_cast<const char*>(data.data()) << std::endl;
        }

        void AsioSession::sendEvent(unsigned eventid, socket::BufferBase_ptr buffer)
        {
            socket::BufferBaseVector_ptr dataBufs = boost::make_shared<socket::BufferBaseVector>();
            dataBufs->push_back(buffer);
            sendEvent(eventid, dataBufs);
        }

        void AsioSession::sendEvent(unsigned eventid, socket::BufferBaseVector_ptr dataBufs)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            sendHeader(eventid, dataBufs->size());
            BOOST_FOREACH(socket::BufferBase_ptr buffer, *dataBufs)
            {
                sendData(*(buffer->serialize()));
            }
        }

        void AsioSession::receiveEvents()
        {
            boost::asio::async_read(*_sock,
                                    boost::asio::buffer(_headerBuf),
                                    boost::asio::transfer_all(),
                                    boost::bind(&AsioSession::onHeaderReceived,
                                                this, _1, _2));
        }

        void AsioSession::onHeaderReceived(const boost::system::error_code & error,
                                           std::size_t bytesTransferred)
        {
            //std::cerr << "onHeaderReceived()" << std::endl;
            if (error == boost::asio::error::eof)
            {
                _disconnectHandler(shared_from_this());
                return;
            }
            else if (error)
            {
                if(!_sock->is_open())
                {
                    // This will happen if we close() the session, because
                    // the async_read operation will get a "Operation canceled"
                    // exception.
                    return;
                }
                BOOST_THROW_EXCEPTION(ReceiveHeaderError()
                                      << BoostSystemCondition(error.default_error_condition().message())
                                      << BoostSystemMsg(error.message()));
            }
            else if(bytesTransferred != sizeof(uint32_t)*2)
            {
                throw std::runtime_error("bytesTransferred != sizeof(uint32_t)*2"); // This is a bug, because transfer_all() should make this impossible.
            }
            //std::cerr << "onHeaderReceived() no errors" << std::endl;

            uint32_t eventid = ntohl(_headerBuf[0]);
            uint32_t numElements = ntohl(_headerBuf[1]);
            //std::cerr << "onHeaderReceived() eventid:" << eventid
                //<< ", numElements:" << numElements << std::endl;
            _receiveLock.lock();
            //std::cerr << "onHeaderReceived() obtained lock" << std::endl;
            try
            {
                socket::MutableBufferVector_ptr dataBufs = receiveAllData(numElements);
                //std::cerr << "onHeaderReceived() received all data" << std::endl;
                socket::ReceiveEvent event(eventid, dataBufs);
                _allEventsHandler(shared_from_this(), event);
                //std::cerr << "onHeaderReceived() invoked allEventsHandler" << std::endl;
                _receiveLock.unlock();
            }
            catch(boost::system::system_error& error)
            {
                _receiveLock.unlock();
                BOOST_THROW_EXCEPTION(ReceiveDataBoostSystemError()
                                      << BoostSystemCondition(error.code().default_error_condition().message())
                                      << BoostSystemMsg(error.code().message()));
            } catch(std::exception& e) {
                _receiveLock.unlock();
                BOOST_THROW_EXCEPTION(ReceiveDataStdError()
                                      << StdErrorMsg(e.what()));
            } catch(...) {
                _receiveLock.unlock();
                BOOST_THROW_EXCEPTION(ReceiveDataUnknownError());
            }
            //std::cerr << "onHeaderReceived() finished!" << std::endl;
            receiveEvents();
        }

        socket::MutableBuffer_ptr AsioSession::receiveData()
        {
            boost::array<uint32_t, 1> dataSizeBuf;
            int bytes_read = boost::asio::read(*_sock,
                                               boost::asio::buffer(dataSizeBuf),
                                               boost::asio::transfer_all());
            assert(bytes_read == sizeof(uint32_t));
            uint32_t dataSize = ntohl(dataSizeBuf[0]);

            boost::shared_array<char> data = boost::shared_array<char>(new char[dataSize]);
            boost::asio::read(*_sock,
                              boost::asio::buffer(data.get(), dataSize),
                              boost::asio::transfer_all());

            socket::MutableBuffer_ptr mb;
            mb = boost::make_shared<socket::MutableBuffer>(data, dataSize);
            return mb;
        }

        socket::MutableBufferVector_ptr AsioSession::receiveAllData(unsigned numElements)
        {
            socket::MutableBufferVector_ptr dataBufs = boost::make_shared<socket::MutableBufferVector>();
            for(int i = 0; i < numElements; i++)
            {
                socket::MutableBuffer_ptr mb = receiveData();
                dataBufs->push_back(mb);
            }
            return dataBufs;
        }

        void AsioSession::close()
        {
            boost::lock_guard<boost::mutex> lock(_closeMutex);
            static int c = 0;
            if(_sock->is_open())
            {
                boost::system::error_code ignored_error;
                _sock->shutdown(boost::asio::ip::tcp::socket::shutdown_send, ignored_error);
                _sock->shutdown(boost::asio::ip::tcp::socket::shutdown_receive, ignored_error);
                _sock->close(ignored_error);
            }
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
