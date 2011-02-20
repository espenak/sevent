#pragma once
#include "EventData.h"
#include "Service.h"
#include "Address.h"
#include "Service.h"
#include "Listener.h"
#include "Session.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace sevent
{
    namespace socket
    {
        class Facade;
        typedef boost::shared_ptr<Facade> Facade_ptr;

        class Facade
        {
            public:
                static Facade_ptr make();
            public:
                typedef boost::function<void(
                    Facade_ptr facade,
                    Session_ptr session,
                    EventData eventData)> allEventsHandler_t;
                typedef boost::function<void(Facade_ptr facade)> workerThread_t;
            public:
                virtual ~Facade() {};
                virtual Service_ptr service() = 0;
                virtual void setWorkerThreads(unsigned count,
                                              allEventsHandler_t allEventsHandler) = 0;
                virtual void setWorkerThreads(unsigned count,
                                              workerThread_t workerThreadHandler,
                                              allEventsHandler_t allEventsHandler) = 0;
                virtual Listener_ptr listen(Address_ptr address) = 0;
                virtual Session_ptr connect(Address_ptr address) = 0;
                virtual void joinAllWorkerThreads() = 0;
        };
    }
}
