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
                  Event_ptr event)
{
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr data = event->first<String_ptr>(serialize::String);
}


int main(int argc, const char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0]
                  << " <address>" << std::endl;
        return 1;
    }
    std::string address(argv[1]);

    Facade_ptr facade = Facade::make();
    HandlerMap_ptr eventHandlerMap = HandlerMap::make();
    eventHandlerMap->addEventHandler("example::Msg",
                                     helloHandler);
    facade->setWorkerThreads(5,
                             boost::bind(simpleAllEventsHandler,
                                         eventHandlerMap,
                                         _1, _2, _3));
    facade->listen(Address::make(address));
    facade->joinAllWorkerThreads();

    return 0;
}
