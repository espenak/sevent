#include <iostream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include "sevent/sevent.h"
#include "Instrumentation.h"

using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;


boost::mutex receivedEventsLock;
unsigned receivedEvents = 0;
unsigned expectedEvents;
void instrumentationHandler(Facade_ptr facade,
                   Session_ptr session,
                   Event_ptr event)
{
    Instrumentation_ptr p = event->first<Instrumentation_ptr>(serialize::Boost<Instrumentation>());
    //std::cout << "Instrumentation-event received! "
        //<< p->id << ":" << p->systemLoad << std::endl;
    boost::lock_guard<boost::mutex> lock(receivedEventsLock);
    receivedEvents ++;
    if(receivedEvents == expectedEvents)
    {
        facade->service()->stop();
    }
}


int main(int argc, const char *argv[])
{
    if(argc < 4)
    {
        std::cout << "Usage: " << argv[0]
            << " <ip> <port> <expectedEvents>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);
    expectedEvents = boost::lexical_cast<unsigned>(argv[3]);

    Facade_ptr facade = Facade::make();
    HandlerMap_ptr eventHandlerMap = HandlerMap::make();
    eventHandlerMap->addEventHandler("heavy::Instrumentation",
                                     instrumentationHandler);
    facade->setWorkerThreads(5,
            boost::bind(simpleAllEventsHandler,
                        eventHandlerMap,
                        _1, _2, _3));
    Listener_ptr listener1 = facade->listen(Address::make(host, port));
    facade->joinAllWorkerThreads();

    return 0;
}
