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
#include "sevent/serialize/Boost.h"
#include "SerializablePerson.h"


using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;

void helloHandler(Facade_ptr facade,
                  Session_ptr session,
                  Event_ptr event)
{
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr data = event->first<String_ptr>(serialize::String);
    std::cout << "EventId=" << event->eventid()
        << "  Data=" << *data << std::endl;
}


void personHandler(Facade_ptr facade,
                   Session_ptr session,
                   Event_ptr event)
{
    Person_ptr p = event->first<Person_ptr>(serialize::Boost<Person>());
    std::cout << "Person-event received! "
        << p->name << ":" << p->age << std::endl;
}


void dieHandler(Facade_ptr facade,
                Session_ptr session,
                Event_ptr event)
{
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}



int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: " << argv[0]
            << " <ip> <port>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    Facade_ptr facade = Facade::make();

    // Setup the eventhandlers
    HandlerMap_ptr eventHandlerMap = HandlerMap::make();
    eventHandlerMap->addEventHandler("example::Msg",
                                     helloHandler);
    eventHandlerMap->addEventHandler("example::Person",
                                     personHandler);
    eventHandlerMap->addEventHandler("example::Die",
                                     dieHandler);

    // Start 5 worker threads,
    // and use the handler above for incoming events.
    facade->setWorkerThreads(5,
            boost::bind(simpleAllEventsHandler,
                        eventHandlerMap,
                        _1, _2, _3));

    // Create a listening socket.
    Listener_ptr listener1 = facade->listen(Address::make(host, port));

    // Wait for all work to finish. In this example this will happen
    // when the dieHandler calls facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
