#pragma once
#include "sevent/event/Event.h"
#include "Address.h"
#include "Service.h"
#include "Listener.h"
#include "Session.h"
#include "SessionRegistry.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <map>

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
                    event::Event_ptr event)> allEventsHandler_t;

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

                /** Send an event using the given session, using local transfer
                 * if the session was created with this facade.
                 *
                 * Shortcut for:
                 * \code
                 * if(facade->isLocalSession(session)) {
                 *     facade->invokeAllEventsHandler(session, event); // Invoke handler directly
                 * } else {
                 *     session->sendEvent(event); // Serialize message, and send it via the session (socket communication)
                 * }
                 * \endcode
                 * */
                virtual void sendEvent(Session_ptr session,
                                       event::Event_ptr event) = 0;

                /** Create session with the server at the given address, and
                 * send the given event via the newly created session. */
                void sendEvent(Address_ptr address, event::Event_ptr event);

                /** Send the given event directly to the all-events-handler in
                 * this facade. */
                virtual void invokeAllEventsHandler(Session_ptr session,
                                                    event::Event_ptr event) = 0;

                void removeListener(const Address& address);

                /** Check if the given session is a local session.
                 *
                 * @return true of it is a local session.
                 */
                bool isLocalSession(Session_ptr session);

            protected:
                void saveListener(const Address& address,
                                  Listener_ptr listener);

            private:
                std::map<Address, Listener_ptr> _listeners;
        };
    }
}
