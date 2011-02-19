#pragma once
#include "EventData.h"
#include "SocketService.h"
#include "Address.h"
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
            typedef boost::function<void(SocketFacade_ptr facade,
                                         EventData eventData,
                                         SocketService_ptr socketService)> allEventsHandler_t;
            typedef boost::function<void(SocketFacade_ptr facade)> workerThread_t;
        public:
            virtual ~SocketFacade() {};
            virtual void setWorkerThreads(unsigned count) = 0;
            virtual void setWorkerThreads(unsigned count,
                                          workerThread_t workerThread) = 0;
            virtual void setAllEventsHandler(allEventsHandler_t allEventsHandler) = 0;
            virtual void listen(Address_ptr address) = 0;
        };
    }
}
