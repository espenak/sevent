#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"


using namespace sevent;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
enum EventIds
{
    ECHO_ID = 10,
    ECHO_RESPONSE_ID = 11,
    DIE_ID = 20,
    NUM_ID = 30,
};


// echoHandler() and dieHandler() handles received events.
// All data is downloaded before the handler is called.
//
// Notice that all handlers get a ReceiveEvent object, which
// contains the eventid. This means that we can use the same handler
// for multiple events!
void echoHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  socket::ReceiveEvent& event)
{
    char* data = event.firstData<char*>();
    unsigned size = event.firstDataSize();
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "==================================" << std::endl
            << "HELLO-event received!" << std::endl
            << "Event id:  " << event.eventid() << std::endl
            << "Data:      " << data << std::endl
            << "Data size: " << size << std::endl
            << "==================================" << std::endl;
    }
    session->sendEvent(ECHO_RESPONSE_ID, socket::ConstBuffer(data, size));
}

void numHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  socket::ReceiveEvent& event)
{

    // Notice that we have to popBackAndDecode in reverse order
    socket::MutableBuffer_ptr int32numsBuf = endiansafe::popBackAndDecode<int32_t>(event);
    int32_t* int32nums = int32numsBuf->data<int32_t*>();
    unsigned int32numsSize = int32numsBuf->numElements<int32_t>();

    socket::MutableBuffer_ptr uint16numsBuf = endiansafe::popBackAndDecode<uint16_t>(event);
    uint16_t* uint16nums = uint16numsBuf->data<uint16_t*>();
    unsigned uint16numsSize = uint16numsBuf->numElements<uint16_t>();

    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "==================================" << std::endl
            << "NUM-event received!" << std::endl
            << "Event id:  " << event.eventid() << std::endl
            << "Data:" << std::endl;
        for(int i = 0; i < uint16numsSize; i++)
        {
            std::cout << std::setw(8) << uint16nums[i] << " ";
        }
        std::cout << std::endl;
        for(int i = 0; i < int32numsSize; i++)
        {
            std::cout << std::setw(8) << int32nums[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "==================================" << std::endl;
    }
    session->sendEvent(ECHO_RESPONSE_ID, socket::ConstBuffer("OK", 3));
}

void dieHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                socket::ReceiveEvent& event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}


//
// A very simple "all events handler" which just triggers events from an
// eventmap. This could of course be a class with operator() if we needed
// something more complex.
void allEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                      socket::Facade_ptr facade,
                      socket::Session_ptr session,
                      socket::ReceiveEvent& event)
{
    eventHandlerMap->triggerEvent(facade, session, event);
}

void disconnectHandler(socket::SessionRegistry_ptr sessionRegistry,
                       socket::Session_ptr session)
{

    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << session->getRemoteEndpointAddress()
            << " disconnected" << std::endl;
    }
    sessionRegistry->remove(session);
}


int main(int argc, const char *argv[])
{
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(ECHO_ID, echoHandler);
    eventHandlerMap->addEventHandler(DIE_ID, dieHandler);
    eventHandlerMap->addEventHandler(NUM_ID, numHandler);

    // Setup the disconnect handler
    facade->sessionRegistry()->setDisconnectHandler(boost::bind(disconnectHandler,
                                                                facade->sessionRegistry(),
                                                                _1));

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(5, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));

    // Create a listening socket.
    socket::Listener_ptr listener = facade->listen(socket::Address::make(host, port));

    // Wait for all work to finish. In this example this will happen
    // when the dieHandler calls facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
