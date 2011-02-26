#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {

        /** A service handles IO-events.
         *
         * Internally, the Connector and Listener register their sockets
         * to the service, and worker-threads listen for events on these
         * sockets.
         */
        class Service
        {
            public:
                virtual ~Service()
                {
                }

                /** Run the service object's event processing loop. */
                virtual void run() = 0;

                /** Stop the service object's event processing loop. */
                virtual void stop() = 0;
        };
        typedef boost::shared_ptr<Service> Service_ptr;

    } // namespace socket
} // namespace sevent
