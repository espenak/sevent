#pragma once
#include "EventData.h"
#include "SocketService.h"
#include "Address.h"
#include "SocketService.h"
#include "SocketListener.h"
#include "SocketSession.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace sevent
{
    namespace socket
    {
        class SocketFacade;
        typedef boost::shared_ptr<SocketFacade> SocketFacade_ptr;

        class SocketFacade
        {
        public:
            typedef boost::function<void(
                    SocketFacade_ptr facade,
                    SocketSession_ptr session,
                    EventData eventData)> allEventsHandler_t;
            typedef boost::function<void(SocketFacade_ptr facade)> workerThread_t;
        public:
            virtual ~SocketFacade() {};
            virtual SocketService_ptr service() = 0;
            virtual void setWorkerThreads(unsigned count,
                    allEventsHandler_t allEventsHandler) = 0;
            virtual void setWorkerThreads(unsigned count,
                    workerThread_t workerThreadHandler,
                    allEventsHandler_t allEventsHandler) = 0;
            virtual SocketListener_ptr listen(Address_ptr address) = 0;
            virtual SocketSession_ptr connect(Address_ptr address) = 0;
            virtual void joinAllWorkerThreads() = 0;
        };
    }
}
