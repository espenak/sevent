#pragma once
#include "SocketFacade.h"
#include "AsioSocketService.h"
#include "SocketSessionRegistry.h"
#include "AsioSocketConnector.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace sevent
{
    namespace socket
    {
        class AsioSocketFacade;
        typedef boost::shared_ptr<AsioSocketFacade> AsioSocketFacade_ptr;
        
        class AsioSocketFacade : public SocketFacade,
            public boost::enable_shared_from_this<AsioSocketFacade>
        {
        public:
            static AsioSocketFacade_ptr make();
        public:
            AsioSocketFacade();
        public:
            virtual ~AsioSocketFacade();
            virtual SocketService_ptr service();
            virtual void setWorkerThreads(unsigned count,
                    allEventsHandler_t allEventsHandler);
            virtual void setWorkerThreads(unsigned count,
                    workerThread_t workerThreadHandler,
                    allEventsHandler_t allEventsHandler);
            virtual SocketListener_ptr listen(Address_ptr address);
            virtual SocketSession_ptr connect(Address_ptr address);
            virtual void joinAllWorkerThreads();
        private:
            static void defaultWorkerThreadHandler(SocketFacade_ptr facade);
        private:
            AsioSocketService_ptr _service;
            SocketConnector_ptr _connector;
            SocketSessionRegistry_ptr _sessionRegistry;

            workerThread_t _workerThreadHanler;
            allEventsHandler_t _allEventsHandler;
            boost::thread_group _worker_threads;
        };
    }
}
