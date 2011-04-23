#include <iostream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"
#include "SerializablePerson.h"

using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;
using namespace sevent::datastruct;

void helloHandler(Facade_ptr facade,
                  Session_ptr session,
                  Event_ptr event) {
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr data = event->first<String_ptr>(
                            serialize::String);
    std::cout << "EventId=" << event->eventid()
        << "  Data=" << *data << std::endl;
}

void personHandler(Facade_ptr facade,
                   Session_ptr session,
                   Event_ptr event) {
    Person_ptr p = event->first<Person_ptr>(
                            serialize::Boost<Person>());
    std::cout << "Person-event received! "
        << p->name << ":" << p->age << std::endl;
}

void uint32ArrayHandler(Facade_ptr facade,
                        Session_ptr session,
                        Event_ptr event) {
    Uint32SharedArray_ptr arrayContainer = event->first<Uint32SharedArray_ptr>(serialize::Uint32SharedArray);
    boost::shared_array<uint32_t> array = arrayContainer->array();
    std::cout << "Shared array-event received! " << std::endl;
    for(int i = 0; i < arrayContainer->size(); i++) {
        std::cout << "sharedArray[" << i << "] = "
            << array[i] << std::endl;
    }
}

void dieHandler(Facade_ptr facade,
                Session_ptr session,
                Event_ptr event) {
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}

int main(int argc, const char *argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0]
            << " <address>" << std::endl;
        return 1;
    }
    std::string address(argv[1]);

    Facade_ptr facade = Facade::make();

    // Setup the eventhandlers
    HandlerMap_ptr eventHandlerMap = HandlerMap::make();
    eventHandlerMap->addEventHandler("example::Msg",
                                     helloHandler);
    eventHandlerMap->addEventHandler("example::Person",
                                     personHandler);
    eventHandlerMap->addEventHandler("example::Array",
                                     uint32ArrayHandler);
    eventHandlerMap->addEventHandler("example::Die",
                                     dieHandler);

    // Start 5 worker threads,
    // and use the handler above for incoming events.
    facade->setWorkerThreads(5,
            boost::bind(simpleAllEventsHandler,
                        eventHandlerMap,
                        _1, _2, _3));

    // Create a listening socket.
    facade->listen(Address::make(address));

    // Wait for all work to finish. In this example this will
    // happen when the dieHandler calls
    // facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
