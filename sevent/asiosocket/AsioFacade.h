#pragma once
#include "sevent/socket/Facade.h"
#include "sevent/socket/SessionRegistry.h"
#include "AsioService.h"
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
                static void defaultWorkerThreadHandler(socket::Facade_ptr facade);
            public:
                AsioFacade();
            public:
                virtual ~AsioFacade();
                virtual socket::Service_ptr service();
                virtual socket::SessionRegistry_ptr sessionRegistry();


                /** Shortcut for setWorkerThreads(numberOfWorkerThreads,
                 * defaultWorkerThreadHandler, allEventsHandler) */
                virtual void setWorkerThreads(unsigned numberOfWorkerThreads,
                                              allEventsHandler_t allEventsHandler);


                virtual void setWorkerThreads(unsigned numberOfWorkerThreads,
                                              workerThread_t workerThreadHandler,
                                              allEventsHandler_t allEventsHandler);
                virtual socket::Listener_ptr listen(socket::Address_ptr address);
                virtual socket::Session_ptr connect(socket::Address_ptr address);
                virtual void joinAllWorkerThreads();
                virtual void sendEvent(socket::Session_ptr session,
                                       event::Event_ptr event);
                virtual void invokeAllEventsHandler(socket::Session_ptr session,
                                                    event::Event_ptr event);

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
