#pragma once
#include "Facade.h"
#include "AsioService.h"
#include "SessionRegistry.h"
#include "AsioConnector.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace sevent
{
    namespace socket
    {
        class AsioFacade;
        typedef boost::shared_ptr<AsioFacade> AsioFacade_ptr;

        class AsioFacade : public Facade,
            public boost::enable_shared_from_this<AsioFacade>
        {
            public:
                static AsioFacade_ptr make();
            public:
                AsioFacade();
            public:
                virtual ~AsioFacade();
                virtual Service_ptr service();
                virtual void setWorkerThreads(unsigned count,
                                              allEventsHandler_t allEventsHandler);
                virtual void setWorkerThreads(unsigned count,
                                              workerThread_t workerThreadHandler,
                                              allEventsHandler_t allEventsHandler);
                virtual Listener_ptr listen(Address_ptr address);
                virtual Session_ptr connect(Address_ptr address);
                virtual void joinAllWorkerThreads();

            private:
                static void defaultWorkerThreadHandler(Facade_ptr facade);
            private:
                AsioService_ptr _service;
                Connector_ptr _connector;
                SessionRegistry_ptr _sessionRegistry;

                workerThread_t _workerThreadHanler;
                allEventsHandler_t _allEventsHandler;
                boost::thread_group _worker_threads;
        };
    }
}
