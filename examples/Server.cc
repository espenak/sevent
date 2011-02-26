/** A server listening for 2 events: hello(print input) and die(stop server). */
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"


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
    std::cout << "Data:      " << event.first()->data<char>() << std::endl;
    std::cout << "Data size: " << event.first()->size() << std::endl;
    std::cout << "==================================" << std::endl;
}



void dieHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                socket::ReceiveEvent& event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}



int main(int argc, const char *argv[])
{
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(HELLO_ID, helloHandler);
    eventHandlerMap->addEventHandler(DIE_ID, dieHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(1, boost::bind(event::simpleAllEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));

    // Create a listening socket.
    socket::Listener_ptr listener1 = facade->listen(socket::Address::make(host, port));

    // Wait for all work to finish. In this example this will happen
    // when the dieHandler calls facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
