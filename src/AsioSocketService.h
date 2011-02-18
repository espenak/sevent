#pragma once

#include "SocketService.h"
#include <boost/asio.hpp>

namespace socketevent
{

class AsioSocketService: public SocketService
{
    public:
        AsioSocketService();
        virtual ~AsioSocketService();
        virtual void run();

    private:
        boost::asio::io_service _io_service;
        boost::asio::io_service::work _work;
};

}
