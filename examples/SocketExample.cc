#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket.h"


using namespace sevent::socket;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine

void allEventsHandler(Facade_ptr facade,
                      Session_ptr session, ReceiveEvent& event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "Event " << event.eventid() << " received!" << std::endl;
    if (event.eventid() == 2020)
    {
        std::cout << "Stopping service handler ..." << std::endl;
        facade->service()->stop();
    }
    delete[] event.data();
}


int main(int argc, const char *argv[])
{
    Facade_ptr facade = Facade::make();
    facade->setWorkerThreads(5, allEventsHandler); // Start 5 worker threads, and use the handler above for incoming events.
    Listener_ptr listener1 = facade->listen(Address::make("127.0.0.1", 9091));
    Listener_ptr listener2 = facade->listen(Address::make("127.0.0.1", 9092));

    Session_ptr session1 = facade->connect(Address::make("127.0.0.1", 9091));
    Session_ptr session2 = facade->connect(Address::make("127.0.0.1", 9092));

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
    facade->joinAllWorkerThreads();

    return 0;
}
