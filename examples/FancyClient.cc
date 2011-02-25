#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"


using namespace sevent;
enum EventIds
{
    ECHO_ID = 10,
    ECHO_RESPONSE_ID = 11,
    DIE_ID = 20,
    NUM_ID = 30,
    NUM_RESPONSE_ID= 31
};

int expectedResponseCount;
void quitIfFinished(socket::Facade_ptr facade,
                    socket::Session_ptr session)
{
    static int responseCount = 0;
    responseCount ++;
    if(responseCount == expectedResponseCount)
    {
        session->sendEvent(DIE_ID, socket::ConstBuffer(0, 0));
        facade->service()->stop();
    }
}


boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
void echoResponseHandler(socket::Facade_ptr facade,
                         socket::Session_ptr session,
                         socket::ReceiveEvent& event)
{
    char* data = event.firstData<char*>();
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "Hello-response-event received!" << std::endl
        << "      " << data << std::endl;
    quitIfFinished(facade, session);
}

void numResponseHandler(socket::Facade_ptr facade,
                         socket::Session_ptr session,
                         socket::ReceiveEvent& event)
{
    char* data = event.firstData<char*>();
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "Num-response-event received!" << std::endl
        << "      " << data << std::endl;
    quitIfFinished(facade, session);
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
    eventHandlerMap->addEventHandler(NUM_RESPONSE_ID, numResponseHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(5, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));
    socket::Listener_ptr listener = facade->listen(socket::Address::make(host, 2020));

    // Make a session and show both sides of the communication
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "Connected to " << session->getRemoteEndpointAddress()
            << " as " << session->getLocalEndpointAddress() << std::endl;
    }

    // Lets send a couple of events! Note that the received order is not
    // guaranteed.
    
    session->sendEvent(ECHO_ID, socket::ConstBuffer("Hello", 6));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("(somewhat) cruel", 17));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("World", 6));

    uint16_t someU16numbers[] = {10, 20, 30, 40, 50, 60};
    //socket::ConstBuffer someU16buf(someU16numbers, sizeof(uint16_t) * 6);
    socket::ConstBufferVector numvec;
    endiansafe::pushBack(numvec, someU16numbers, 6);
    session->sendEvent(NUM_ID, numvec);

    expectedResponseCount = 4;

    facade->joinAllWorkerThreads();

    return 0;
}
