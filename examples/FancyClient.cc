#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "sevent/socket.h"
#include "sevent/EventHandlerMap.h"


using namespace sevent;
enum EventIds
{
    ECHO_ID = 10,
    ECHO_RESPONSE_ID = 11,
    DIE_ID = 20
};


boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
std::string response;
void echoResponseHandler(socket::Facade_ptr facade,
                         socket::Session_ptr session,
                         socket::ReceiveEvent& event)
{
    char* data = event.firstData<char*>();
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "Hello-response-event received!" << std::endl
        << "      " << data << std::endl;
    //response.append(data);
    //std::cout << response << std::endl;
    //if(response == std::string("HelloCruelWorld"))
    //{
        //session->sendEvent(DIE_ID, socket::ConstBuffer(0, 0));
    //}
}

void allEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                      socket::Facade_ptr facade,
                      socket::Session_ptr session,
                      socket::ReceiveEvent& event)
{
    eventHandlerMap->triggerEvent(facade, session, event);
}


int main(int argc, const char *argv[])
{
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(ECHO_RESPONSE_ID, echoResponseHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(5, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));
    socket::Listener_ptr listener1 = facade->listen(socket::Address::make(host, 2020));

    // Make 2 sessions, one for each listening socket.
    // Notice that two of them is to the same listening socket!
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed.
    session->sendEvent(ECHO_ID, socket::ConstBuffer("Hello", 6));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("Cruel", 6));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("World", 6));

    facade->joinAllWorkerThreads();

    return 0;
}
