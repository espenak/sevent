/** A server listening for 3 types of events:
 * - hello(print input)
 * - person(deserialize and print person)
 * - die(stop server)
 */
#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"
#include "sevent/serialize/String.h"
//#include "SerializablePerson.h"


using namespace sevent;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
enum EventIds
{
    HELLO_ID = 10,
    PERSON_ID = 15,
    DIE_ID = 20
};


// helloHandler(), personHandler() and dieHandler() handles received events.
// All data is downloaded before the handler is called.
//
// Notice that all handlers get a ReceiveEvent object, which
// contains the eventid. This means that we can use the same handler
// for multiple events!
void helloHandler(socket::Facade_ptr facade,
                  socket::Session_ptr session,
                  event::Event_ptr event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr data = event->first<String_ptr>(serialize::String);
    std::cout << "==================================" << std::endl;
    std::cout << "Hello-event received!" << std::endl;
    std::cout << "Event id:  " << event->eventid() << std::endl;
    std::cout << "Data:      " << *data << std::endl;
    std::cout << "Sender: " << session->getRemoteEndpointAddress() << std::endl;
    std::cout << "Receiver: " << session->getLocalEndpointAddress() << std::endl;
    std::cout << "==================================" << std::endl;
}


//void personHandler(socket::Facade_ptr facade,
                   //socket::Session_ptr session,
                   //socket::ReceiveEvent& event)
//{
    //Person p;
    //sevent::boostserialize::fromString(p, event.first()->data<char>());
    //boost::lock_guard<boost::mutex> lock(stream_lock);
    //std::cout << "### Person-event received: "
        //<< p.name << ":" << p.age << " ###" << std::endl;
//}


//void dieHandler(socket::Facade_ptr facade,
                //socket::Session_ptr session,
                //socket::ReceiveEvent& event)
//{
    //boost::lock_guard<boost::mutex> lock(stream_lock);
    //std::cout << "*** DIE-event received ***" << std::endl;
    //facade->service()->stop();
//}



int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(HELLO_ID, helloHandler);
    //eventHandlerMap->addEventHandler(PERSON_ID, personHandler);
    //eventHandlerMap->addEventHandler(DIE_ID, dieHandler);

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
