#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/SocketService.h"
#include "sevent/socket/AsioSocketService.h"
#include "sevent/socket/AsioSocketListener.h"
#include "sevent/socket/AsioSocketConnector.h"
#include "sevent/socket/WorkerThread.h"

using namespace sevent::socket;

void allEventsHandler(SocketSession_ptr session, EventData eventData,
        SocketService_ptr service)
{
    std::cout << "Event " << eventData.eventid() << " received!" << std::endl;
    if (eventData.eventid() == 30)
    {
        std::cout << "Stopping service handler ..." << std::endl;
        service->stop();
    }
}

int main(int argc, const char *argv[])
{
    // The service handles IO events
    AsioSocketService_ptr service = AsioSocketService::make();

    // Start worker threads
    // Worker threads handle IO events like incoming connections and received data
    int workerThreadCount = 2; // number of socket event handler threads
    boost::thread_group worker_threads;
    for (int x = 0; x < workerThreadCount; ++x)
    {
        worker_threads.create_thread(boost::bind(boost::ref(
                exceptionHandlingWorkerThread), service));
    }

    // Each incoming or outgoing connection get a session. They are all stored in a
    // registry.
    SocketSessionRegistry_ptr socketSessionRegistry =
            SocketSessionRegistry::make(boost::bind(allEventsHandler, _1, _2,
                    service));

    // The listener listens for new connections, creates sessions and adds them to the registry.
    AsioSocketListener listener(service, socketSessionRegistry);
    Address_ptr serverAddr = Address::make("localhost", "9090");
    listener.listen(serverAddr);
    std::cout << "Listening on " << std::endl;

    // Clients connect using a SocketConnector.
    AsioSocketConnector client(service, socketSessionRegistry);
    SocketSession_ptr session = client.connect(Address::make("localhost",
            "9090"));

    // Lets send a couple of events!
    char hello[6] = { 'h', 'e', 'l', 'l', 'o', '\0' };
    session->sendEvent(EventData(10, hello, 6));
    char die[5] = { 'd', 'i', 'e', '!', '\0' };
    session->sendEvent(EventData(20, die, 5));

    // Always nice to know who you are communicating with..
    std::cout <<
            "Local: " << session->getLocalEndpointInfo() <<
            " Remote: " << session->getRemoteEndpointInfo() << std::endl;

    // Wait for all work to finish. In this example this will happen
    // when the second message (with id:20) has been handled by allEventsHandler
    worker_threads.join_all();

    return 0;
}
