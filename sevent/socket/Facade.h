#pragma once
#include "ReceiveEvent.h"
#include "Service.h"
#include "Address.h"
#include "Service.h"
#include "Listener.h"
#include "Session.h"
#include "SessionRegistry.h"
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

                /** Events are sent to a callback of this type defined in
                 * setWorkerThreads().
                 * 
                 * \warning The data-buffer in ReceiveEvent::data() is not
                 * freed automatically, so it is up to the handler to make sure
                 * no memory leaks occur (by calling delete[] event.data()). */
                typedef boost::function<void(
                    Facade_ptr facade,
                    Session_ptr session,
                    ReceiveEvent& event)> allEventsHandler_t;

                typedef boost::function<void(Facade_ptr facade)> workerThread_t;
            public:
                virtual ~Facade() {};
                virtual Service_ptr service() = 0;
                virtual SessionRegistry_ptr sessionRegistry() = 0;

                /** Shortcut for setWorkerThreads() with a implementation
                 * spesific default workerThreadHandler. */
                virtual void setWorkerThreads(unsigned numberOfWorkerThreads,
                                              allEventsHandler_t allEventsHandler) = 0;


                /**
                 * @param numberOfWorkerThreads Number of worker threads
                 *      started.
                 * @param allEventsHandler Called to handle incoming events.
                 * @param workerThreadHandler This function is started in
                 *      numberOfWorkerThreads separate threads.
                 * */
                virtual void setWorkerThreads(unsigned numberOfWorkerThreads,
                                              workerThread_t workerThreadHandler,
                                              allEventsHandler_t allEventsHandler) = 0;

                /** Create a listening socket and start accepting requests.
                 *
                 * Blocks until a successful listening socket is created.
                 *
                 * @throw std::runtime_error
                 *      If listening on the given address fails.
                 * */
                virtual Listener_ptr listen(Address_ptr address) = 0;


                /** Create a session with the Listener at the given
                 * address.
                 *
                 * Blocks until a connection is established.
                 *
                 * @throw std::runtime_error
                 *      If connecting to the given address fails.
                 * */
                virtual Session_ptr connect(Address_ptr address) = 0;
                
                
                /** Block until all workerthreads are finished.
                 * 
                 * Use service()->stop() (from another thread) to stop the
                 * Service. */
                virtual void joinAllWorkerThreads() = 0;

        };
    }
}
