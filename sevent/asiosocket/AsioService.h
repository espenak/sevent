#pragma once

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include "sevent/socket/Service.h"

namespace sevent
{
    namespace asiosocket
    {

        class AsioService;
        typedef boost::shared_ptr<AsioService> AsioService_ptr;

        class AsioService: public socket::Service
        {
            public:
                static AsioService_ptr make();
            public:
                AsioService();
                virtual ~AsioService();
            public:
                virtual void run();
                virtual void stop();

            public:
                boost::asio::io_service _io_service;
            private:
                boost::asio::io_service::work _work;
        };

    } // namespace asiosocket
} // namespace sevent
