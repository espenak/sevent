#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket.h"
#include "sevent/EventHandlerMap.h"


using namespace sevent;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
enum EventIds
{
    HELLO_ID = 10,
    DIE_ID = 20
};


// helloHandler() and dieHandler() handles received events.
// All data is downloaded before the handler is called.
//
// Notice that all handlers get a ReceiveEvent object, which
// contains the eventid. This means that we can use the same handler
// for multiple events!
void helloHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  socket::ReceiveEvent& event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "==================================" << std::endl;
    std::cout << "Hello-event received!" << std::endl;
    std::cout << "Event id:  " << event.eventid() << std::endl;
    std::cout << "Data:      " << event.first()->data<char*>() << std::endl;
    std::cout << "Data size: " << event.firstDataSize() << std::endl;
    std::cout << "==================================" << std::endl;
}

void dieHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                socket::ReceiveEvent& event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}


// A very simple "all events handler" which just triggers events from an
// eventmap. This could of course be a class with operator() if we needed
// something more complex.
void allEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                      socket::Facade_ptr facade,
                      socket::Session_ptr session,
                      socket::ReceiveEvent& event)
{
    eventHandlerMap->triggerEvent(facade, session, event);
}


int main(int argc, const char *argv[])
{
    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(HELLO_ID, helloHandler);
    eventHandlerMap->addEventHandler(DIE_ID, dieHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(5, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));

    // Create 2 listening sockets.
    socket::Listener_ptr listener1 = facade->listen(socket::Address::make("127.0.0.1", 9091));
    socket::Listener_ptr listener2 = facade->listen(socket::Address::make("127.0.0.1", 9092));

    // Make 2 sessions, one for each listening socket.
    // Notice that two of them is to the same listening socket!
    socket::Session_ptr session1 = facade->connect(socket::Address::make("127.0.0.1", 9091));
    socket::Session_ptr session2 = facade->connect(socket::Address::make("127.0.0.1", 9091));
    socket::Session_ptr session3 = facade->connect(socket::Address::make("127.0.0.1", 9092));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed.
    session1->sendEvent(HELLO_ID, socket::ConstBuffer("Hello", 6));
    session2->sendEvent(HELLO_ID, socket::ConstBuffer("Cruel", 6));
    session3->sendEvent(HELLO_ID, socket::ConstBuffer("World", 6));
    session2->sendEvent(DIE_ID, socket::ConstBuffer(0, 0));

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
    // when the dieHandler calls facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
