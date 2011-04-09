#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"


using namespace sevent;
using sevent::event::Buffer;
using sevent::event::Event;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
typedef boost::shared_ptr<std::string> String_ptr;


// helloHandler() and dieHandler() handles received events.
// All data is downloaded before the handler is called.
//
// Notice that all handlers get a ReceiveEvent object, which
// contains the eventid. This means that we can use the same handler
// for multiple events!
void helloHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  event::Event_ptr event)
{
    String_ptr msg = event->first<String_ptr>(serialize::String);
    boost::lock_guard<boost::mutex> lock(stream_lock);

    std::cout << "==================================" << std::endl;
    std::cout << "Hello-event received!" << std::endl;
    std::cout << "Event id:  " << event->eventid() << std::endl;
    std::cout << "Data:      " << *msg << std::endl;
    std::cout << "==================================" << std::endl;
    facade->service()->stop();
}

void dieHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                event::Event_ptr event)
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
    eventHandlerMap->addEventHandler("sevent::examples::Hello", helloHandler);
    eventHandlerMap->addEventHandler("sevent::examples::Die", dieHandler);

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

    // All data must be deallocated on destruction, and it must have a
    // serializer. Shared pointers are deallocated on destruction, and we have
    // a StringSerializer (which does nothing to the string).
    String_ptr hello = boost::make_shared<std::string>("Hello");

    // Lets send a couple of events! Note that the received order is not
    // guaranteed, so we might die before all messages are received!
    session1->sendEvent(Event::make("sevent::examples::Hello",
                                    Buffer::make(hello, serialize::String)));
    session2->sendEvent(Event::make("sevent::examples::Die"));

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
