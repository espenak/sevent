#pragma once
#include "Facade.h"
#include "AsioService.h"
#include "SessionRegistry.h"
#include "AsioConnector.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace sevent
{
    namespace asiosocket
    {
        class AsioFacade;
        typedef boost::shared_ptr<AsioFacade> AsioFacade_ptr;

        class AsioFacade : public socket::Facade,
            public boost::enable_shared_from_this<AsioFacade>
        {
            public:
                static AsioFacade_ptr make();
            public:
                AsioFacade();
            public:
                virtual ~AsioFacade();
                virtual socket::Service_ptr service();
                virtual void setWorkerThreads(unsigned count,
                                              allEventsHandler_t allEventsHandler);
                virtual void setWorkerThreads(unsigned count,
                                              workerThread_t workerThreadHandler,
                                              allEventsHandler_t allEventsHandler);
                virtual socket::Listener_ptr listen(socket::Address_ptr address);
                virtual socket::Session_ptr connect(socket::Address_ptr address);
                virtual socket::Address_ptr resolve(const std::string& host,
                        unsigned short port);
                virtual void joinAllWorkerThreads();

            private:
                static void defaultWorkerThreadHandler(socket::Facade_ptr facade);
            private:
                AsioService_ptr _service;
                socket::Connector_ptr _connector;
                socket::SessionRegistry_ptr _sessionRegistry;

                workerThread_t _workerThreadHanler;
                allEventsHandler_t _allEventsHandler;
                boost::thread_group _worker_threads;
        };
    }
}
