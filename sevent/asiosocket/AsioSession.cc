#include "AsioSession.h"
#include <iostream>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/foreach.hpp>
#include <arpa/inet.h>
#include "sevent/serialize/Result.h"

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


        void AsioSession::sendEventId(event::eventid_t& eventid)
        {
            event::eventid_t::header_network_type header = eventid.serializeHeader();
            boost::asio::write(*_sock,
                               boost::asio::buffer(&header,
                                                   event::eventid_t::headerSerializedSize()),
                               boost::asio::transfer_all());
            if(event::eventid_t::hasBody())
            {
                event::EventIdBodySerialized bodySerialized = eventid.serializeBody();
                boost::asio::write(*_sock,
                                   boost::asio::buffer(bodySerialized.data,
                                                       bodySerialized.size),
                                   boost::asio::transfer_all());
            }
        }

        event::eventid_t_ptr AsioSession::receiveEventId()
        {
            event::eventid_t::header_type header = event::eventid_t::deserializeHeader(_headerBuf[0]);

            if(event::eventid_t::hasBody())
            {
                uint8_t bodySerializedSize = event::eventid_t::bodySerializedSize(header);
                boost::shared_array<char> body = boost::shared_array<char>(new char[bodySerializedSize]); // Using shared array to avoid memory leaks on transfer exceptions
                int bytes_read = boost::asio::read(*_sock,
                                                   boost::asio::buffer(body.get(),
                                                                       bodySerializedSize),
                                                   boost::asio::transfer_all());
                return event::eventid_t::makeFromNetwork(header, body.get());
            }
            else
            {
                return event::eventid_t::makeFromNetwork(header, NULL);
            }
        }

        void AsioSession::sendNumElements(uint32_t numElements)
        {
            uint32_t numElementsNetworkOrder = htonl(numElements);
            boost::asio::write(*_sock,
                               boost::asio::buffer(&numElementsNetworkOrder,
                                                  sizeof(uint32_t)),
                               boost::asio::transfer_all());
        }

        uint32_t AsioSession::receiveNumElements()
        {
            boost::array<uint32_t, 1> buf;
            int bytes_read = boost::asio::read(*_sock,
                                               boost::asio::buffer(buf),
                                               boost::asio::transfer_all());
            return ntohl(buf[0]);
        }

        void AsioSession::sendData(serialize::BaseResult_ptr data)
        {
            uint32_t sizeNetworkOrder = htonl(data->size());
            boost::asio::write(*_sock,
                               boost::asio::buffer(&sizeNetworkOrder,
                                                   sizeof(uint32_t)),
                               boost::asio::transfer_all());
            boost::asio::write(*_sock,
                               boost::asio::buffer(data->data(), data->size()),
                               boost::asio::transfer_all());
        }

        void AsioSession::sendEvent(event::Event_ptr event)
        {
            boost::lock_guard<boost::mutex> lock(_sendLock);
            sendEventId(event->eventid_object());
            sendNumElements(event->size());
            for(int i = 0; i < event->size(); i++)
            {
                sendData(event->serialize_at(i));
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

        void AsioSession::onHeaderReceived(const boost::system::error_code& error,
                                           std::size_t bytesTransferred)
        {
            if(!handleOnHeaderReceivedErrors(error, bytesTransferred))
            {
                return;
            }
            //std::cerr << "onHeaderReceived()" << std::endl;
            //std::cerr << "onHeaderReceived() no errors" << std::endl;

            //uint32_t eventid = ntohl(_headerBuf[0]);
            //uint32_t numElements = ntohl(_headerBuf[1]);
            //std::cerr << "onHeaderReceived() eventid:" << eventid
                //<< ", numElements:" << numElements << std::endl;

            event::eventid_t_ptr eventid;
            datastruct::MutableCharArrayVector_ptr dataBufs;
            {
                boost::lock_guard<boost::mutex> lock(_receiveLock);
                eventid = receiveEventId();
                uint32_t numElements = receiveNumElements();

                dataBufs = receiveAllDataAndHandleErrors(numElements);
            }

            event::Event_ptr event = event::Event::make(eventid->value(), dataBufs);
            _allEventsHandler(shared_from_this(), event);
            //std::cerr << "onHeaderReceived() finished!" << std::endl;
            receiveEvents();
        }

        bool AsioSession::handleOnHeaderReceivedErrors(const boost::system::error_code& error,
                                                       std::size_t bytesTransferred)
        {
            if (error == boost::asio::error::eof)
            {
                _disconnectHandler(shared_from_this());
                return false;
            }
            else if (error)
            {
                if(!_sock->is_open())
                {
                    // This will happen if we close() the session, because
                    // the async_read operation will get a "Operation canceled"
                    // exception.
                    return false;
                } else
                {
                    BOOST_THROW_EXCEPTION(ReceiveHeaderError()
                                          << BoostSystemCondition(error.default_error_condition().message())
                                          << BoostSystemMsg(error.message()));
                }
            }
            else if(bytesTransferred != event::eventid_t::headerSerializedSize())
            {
                throw std::runtime_error("bytesTransferred is incorrect"); // This is a bug, because transfer_all() should make this impossible.
            }
            return true;
        }

        datastruct::MutableCharArray_ptr AsioSession::receiveData()
        {
            boost::array<uint32_t, 1> dataSizeBuf;
            int bytes_read = boost::asio::read(*_sock,
                                               boost::asio::buffer(dataSizeBuf),
                                               boost::asio::transfer_all());
            assert(bytes_read == sizeof(uint32_t));
            uint32_t dataSize = ntohl(dataSizeBuf[0]);

            char *data = new char[dataSize];
            boost::asio::read(*_sock,
                              boost::asio::buffer(data, dataSize),
                              boost::asio::transfer_all());
            return boost::make_shared<datastruct::MutableCharArray>(data, dataSize);
        }

        datastruct::MutableCharArrayVector_ptr AsioSession::receiveAllData(unsigned numElements)
        {
            datastruct::MutableCharArrayVector_ptr dataBufs = boost::make_shared<datastruct::MutableCharArrayVector>();
            for(int i = 0; i < numElements; i++)
            {
                datastruct::MutableCharArray_ptr buffer = receiveData();
                dataBufs->push_back(buffer);
            }
            return dataBufs;
        }

        datastruct::MutableCharArrayVector_ptr AsioSession::receiveAllDataAndHandleErrors(unsigned numElements)
        {
            try
            {
                return receiveAllData(numElements);
            }
            catch(boost::system::system_error& error)
            {
                BOOST_THROW_EXCEPTION(ReceiveDataBoostSystemError()
                                      << BoostSystemCondition(error.code().default_error_condition().message())
                                      << BoostSystemMsg(error.code().message()));
            } catch(std::exception& e) {
                BOOST_THROW_EXCEPTION(ReceiveDataStdError()
                                      << StdErrorMsg(e.what()));
            } catch(...) {
                BOOST_THROW_EXCEPTION(ReceiveDataUnknownError());
            }
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
