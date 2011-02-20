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

using namespace sevent::socket;

void allEventsHandler(SocketSession_ptr session, EventData eventData,
                      SocketService_ptr service)
{
    std::cout << "Event " << eventData.eventid() << " received!" << std::endl;
    if (eventData.eventid() == 20)
    {
        std::cout << "Stopping service handler ..." << std::endl;
        service->stop();
    }
}


void workerThread(SocketService_ptr service)
{
    try
    {
        service->run();
    }
    catch (boost::exception& e)
    {
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Exception: " << boost::diagnostic_information(e)
                  << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Exception: " << e.what() << std::endl;
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
        worker_threads.create_thread(boost::bind(workerThread, service));
    }

    // Each incoming or outgoing connection get a session. They are all stored in a
    // registry.
    SocketSessionRegistry_ptr socketSessionRegistry = SocketSessionRegistry::make();
    socketSessionRegistry->setAllEventsHandler(boost::bind(allEventsHandler, _1, _2, service));

    // The listener listens for new connections, creates sessions and adds them to the registry.
    // We can have multiple listeners..
    Address_ptr serverAddr1 = Address::make("127.0.0.1", "9091");
    Address_ptr serverAddr2 = Address::make("127.0.0.1", "9092");
    AsioSocketListener listener1(service, socketSessionRegistry);
    listener1.listen(serverAddr1);
    AsioSocketListener listener2(service, socketSessionRegistry);
    listener2.listen(serverAddr2);
    std::cout << "Listening on " << serverAddr1 << std::endl;
    std::cout << "Listening on " << serverAddr2 << std::endl;

    // Clients connect using a SocketConnector.
    AsioSocketConnector client(service, socketSessionRegistry);
    SocketSession_ptr session1 = client.connect(serverAddr1);
    SocketSession_ptr session2 = client.connect(serverAddr2);

    // Lets send a couple of events!
    char hello[6] = { 'h', 'e', 'l', 'l', 'o', '\0' };
    session1->sendEvent(EventData(10, hello, 6));
    char die[5] = { 'd', 'i', 'e', '!', '\0' };
    session2->sendEvent(EventData(20, die, 5));

    // Always nice to know who you are communicating with..
    std::cout <<
              "Local: " << session1->getLocalEndpointAddress() <<
              " Remote: " << session1->getRemoteEndpointAddress() << std::endl;
    std::cout <<
              "Local: " << session2->getLocalEndpointAddress() <<
              " Remote: " << session2->getRemoteEndpointAddress() << std::endl;

    // Wait for all work to finish. In this example this will happen
    // when the second message (with id:20) has been handled by allEventsHandler
    worker_threads.join_all();

    return 0;
}
