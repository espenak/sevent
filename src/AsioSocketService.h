#pragma once

#include "SocketService.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace socketevent
{

class AsioSocketService;
typedef boost::shared_ptr<AsioSocketService> AsioSocketService_ptr;

class AsioSocketService: public SocketService
{
    public:
        static AsioSocketService_ptr make();
    public:
        AsioSocketService();
        virtual ~AsioSocketService();
    public:
        virtual void run();
        virtual void stop();

    public:
        boost::asio::io_service _io_service;
    private:
        boost::asio::io_service::work _work;
};

}
