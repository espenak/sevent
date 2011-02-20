#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/Service.h"
#include "sevent/socket/AsioService.h"
#include "sevent/socket/AsioListener.h"
#include "sevent/socket/AsioConnector.h"

using namespace sevent::socket;
using namespace sevent::asiosocket;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine

void allEventsHandler(Session_ptr session, ReceiveEvent& event,
                      Service_ptr service)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "Event " << event.eventid() << " received!" << std::endl;
    if (event.eventid() == 2020)
    {
        std::cout << "Stopping service handler ..." << std::endl;
        service->stop();
    }
}


void workerThread(Service_ptr service)
{
    try
    {
        service->run();
    }
    catch (boost::exception& e)
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Exception: " << boost::diagnostic_information(e)
                  << std::endl;
    }
    catch (std::exception& e)
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "[" << boost::this_thread::get_id()
                  << "] Exception: " << e.what() << std::endl;
    }
}


int main(int argc, const char *argv[])
{
    // The service handles IO events
    AsioService_ptr service = AsioService::make();

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
    SessionRegistry_ptr socketSessionRegistry = SessionRegistry::make();
    socketSessionRegistry->setAllEventsHandler(boost::bind(allEventsHandler, _1, _2, service));

    // The listener listens for new connections, creates sessions and adds them to the registry.
    // We can have multiple listeners..
    Address_ptr serverAddr1 = Address::make("127.0.0.1", 9091);
    Address_ptr serverAddr2 = Address::make("127.0.0.1", 9092);
    AsioListener listener1(service, socketSessionRegistry);
    listener1.listen(serverAddr1);
    AsioListener listener2(service, socketSessionRegistry);
    listener2.listen(serverAddr2);
    std::cout << "Listening on " << serverAddr1 << std::endl;
    std::cout << "Listening on " << serverAddr2 << std::endl;

    // Clients connect using a Connector.
    AsioConnector client(service, socketSessionRegistry);
    Session_ptr session1 = client.connect(serverAddr1);
    Session_ptr session2 = client.connect(serverAddr2);

    // Lets send a couple of events!
    session1->sendEvent(SendEvent(1010, "hello", 6));
    session2->sendEvent(SendEvent(2020, "die!", 5));

    // Always nice to know who you are communicating with..
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout <<
            "Local: " << session1->getLocalEndpointAddress() <<
            " Remote: " << session1->getRemoteEndpointAddress() << std::endl;
        std::cout <<
            "Local: " << session2->getLocalEndpointAddress() <<
            " Remote: " << session2->getRemoteEndpointAddress() << std::endl;
    }

    // Wait for all work to finish. In this example this will happen
    // when the second message (with id:2020) has been handled by allEventsHandler
    worker_threads.join_all();

    return 0;
}
