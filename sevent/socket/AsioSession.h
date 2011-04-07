#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/exception/all.hpp>
#include <vector>
#include "Session.h"

namespace sevent
{
    namespace asiosocket
    {

        class AsioSession;
        typedef boost::shared_ptr<AsioSession> AsioSession_ptr;


        typedef boost::error_info<struct tag_BoostSystemMsg,std::string> BoostSystemCondition;
        typedef boost::error_info<struct tag_BoostSystemMsg,std::string> BoostSystemMsg;
        typedef boost::error_info<struct tag_StdErrorMsg,std::string> StdErrorMsg;
        struct SessionError: virtual boost::exception, virtual std::exception {};
        struct ReceiveHeaderError: virtual SessionError {};
        struct ReceiveDataError: virtual SessionError {};
        struct ReceiveDataBoostSystemError: virtual ReceiveDataError {};
        struct ReceiveDataStdError: virtual ReceiveDataError {};
        struct ReceiveDataUnknownError: virtual ReceiveDataError {};

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
                //virtual void sendEvent(unsigned eventid);
                virtual void sendEvent(event::Event_ptr event);
                virtual void receiveEvents();
                virtual socket::Address_ptr getLocalEndpointAddress();
                virtual socket::Address_ptr getRemoteEndpointAddress();
            private:
                void sendHeader(unsigned eventid, int numElements);
                void sendData(serialize::BaseResult_ptr data);

                datastruct::MutableCharArray_ptr receiveData();
                datastruct::MutableCharArrayVector_ptr receiveAllData(unsigned numElements);
                void onHeaderReceived(const boost::system::error_code& error,
                                      std::size_t byte_transferred);
            protected:
                virtual void close();
            private:
                socket_ptr _sock;
                unsigned dataBufsReceived;
                boost::mutex _sendLock;
                boost::mutex _receiveLock;
                boost::mutex _closeMutex;
                boost::array<uint32_t, 2> _headerBuf;
        };

    } // namespace asiosocket
} // namespace sevent
