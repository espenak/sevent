#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket.h"
#include "sevent/socket/Buffer.h"
#include "sevent/BoostSerialize.h"
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
    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(HELLO_ID, helloHandler);
    eventHandlerMap->addEventHandler(DIE_ID, dieHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    // Worker threads poll for IO-events, and ends up running event-handlers
    // (registered above), so it is a good idea to start new threads or spawn
    // many worker threads if any event-handler does any time-consuming task.
    facade->setWorkerThreads(5, boost::bind(event::simpleAllEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));

    // Create 2 listening sockets. YES we are running both the server and the
    // client in the same process within the same service (a facade contains a
    // single service, and a service is what receives IO-requests).
    // We could of course have created multiple facades, but it would not have
    // made any difference in this example.
    socket::Listener_ptr listener1 = facade->listen(socket::Address::make("127.0.0.1", 9091));
    socket::Listener_ptr listener2 = facade->listen(socket::Address::make("127.0.0.1", 9092));

    // Make 2 sessions, one for each listening socket.
    // Notice that two of them is to the same listening socket!
    socket::Session_ptr session1 = facade->connect(socket::Address::make("127.0.0.1", 9091));
    socket::Session_ptr session2 = facade->connect(socket::Address::make("127.0.0.1", 9091));
    socket::Session_ptr session3 = facade->connect(socket::Address::make("127.0.0.1", 9092));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed, so we might die before all messages are received!
    boost::shared_ptr<std::string> hello = boost::make_shared<std::string>("Hello");
    boost::shared_ptr<std::string> cruel = boost::make_shared<std::string>("cruel");
    boost::shared_ptr<std::string> world = boost::make_shared<std::string>("world");
    session1->sendEvent(HELLO_ID, socket::Buffer<std::string, boostserialize::Serializer>(hello));
    session2->sendEvent(HELLO_ID, socket::Buffer<std::string, boostserialize::Serializer>(cruel));
    session3->sendEvent(HELLO_ID, socket::Buffer<std::string, boostserialize::Serializer>(world));
    session2->sendEvent(DIE_ID);

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
