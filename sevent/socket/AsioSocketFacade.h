#pragma once
#include "SocketFacade.h"

namespace sevent
{
    namespace socket
    {
        class AsioSocketFacade : public SocketFacade
        {
        public:
            AsioSocketFacade();
            virtual ~AsioSocketFacade();
            virtual void setWorkerThreads(unsigned count);
            virtual void setWorkerThreads(unsigned count,
                                          workerThread_t workerThread);
            virtual void setAllEventsHandler(allEventsHandler_t allEventsHandler);
            virtual void listen(Address_ptr address);
        };
    }
}
