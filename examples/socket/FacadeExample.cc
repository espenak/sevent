#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/AsioSocketFacade.h"


using namespace sevent::socket;

void allEventsHandler(SocketFacade_ptr facade,
        SocketSession_ptr session, EventData eventData)
{
    std::cout << "Event " << eventData.eventid() << " received!" << std::endl;
    if (eventData.eventid() == 20)
    {
        std::cout << "Stopping service handler ..." << std::endl;
        facade->service()->stop();
    }
}


int main(int argc, const char *argv[])
{
    SocketFacade_ptr facade = AsioSocketFacade::make();
    facade->setWorkerThreads(5, allEventsHandler);
    SocketListener_ptr listener1 = facade->listen(Address::make("127.0.0.1", "9091"));
    SocketListener_ptr listener2 = facade->listen(Address::make("127.0.0.1", "9092"));
    
    SocketSession_ptr session1 = facade->connect(Address::make("127.0.0.1", "9091"));
    SocketSession_ptr session2 = facade->connect(Address::make("127.0.0.1", "9092"));

    // Lets send a couple of events!
    char hello[6] = { 'h', 'e', 'l', 'l', 'o', '\0' };
    session1->sendEvent(EventData(10, hello, 6));
    char die[5] = { 'd', 'i', 'e', '!', '\0' };
    session2->sendEvent(EventData(20, die, 5));

    // Always nice to know who you are communicating with..
    std::cout <<
            "Local: " << session1->getLocalEndpointAddress() <<
            " Remote: " << session1->getRemoteEndpointAddress() << std::endl;
    std::cout <<
            "Local: " << session2->getLocalEndpointAddress() <<
            " Remote: " << session2->getRemoteEndpointAddress() << std::endl;

    // Wait for all work to finish. In this example this will happen
    // when the second message (with id:20) has been handled by allEventsHandler
    facade->joinAllWorkerThreads();
    
    return 0;
}
