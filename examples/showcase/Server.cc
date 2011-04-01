#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"
#include "Shared.h"
#include "BoostSerializedAddressBook.h"


using namespace sevent;
boost::mutex streamLock; // Guard the print streams to avoid thread output intertwine
boost::mutex debugLock;
bool debug = false;


void echoHandler(socket::Facade_ptr facade,
                 socket::Session_ptr session,
                 socket::ReceiveEvent& event)
{
    char* data = event.first()->data<char>();
    unsigned size = event.first()->size();
    if(debug)
    {
        boost::lock_guard<boost::mutex> lock(streamLock);
        std::cout << "==================================" << std::endl
            << "ECHO-event received!" << std::endl
            << "Event id:  " << event.eventid() << std::endl
            << "Data:      " << data << std::endl
            << "Data size: " << size << std::endl
            << "==================================" << std::endl;
    }
    session->sendEvent(ECHO_RESPONSE_ID, socket::ConstBuffer(data, size));
}

void toggleDebuggingHandler(socket::Facade_ptr facade,
                            socket::Session_ptr session,
                            socket::ReceiveEvent& event)
{
    {
        boost::lock_guard<boost::mutex> lock(debugLock);
        debug = debug?false:true;
    }
    {
        boost::lock_guard<boost::mutex> lock(streamLock);
        std::cout << "Debugging " << (debug?"enabled": "disabled") << std::endl;
        if(!debug)
            std::cout << "Showing one . for each 1000 NUM message." << std::endl;
    }
}

void numHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  socket::ReceiveEvent& event)
{
    Person spiderman;
    boostserialize::fromString(spiderman, event.first()->data<char>());

    boost::shared_ptr<Person> spiderman = event.first()->data<Person, boostserialize::Serializer>();

    // Notice that we have to popBackAndDecode in reverse order
    Person batman;
    boostserialize::fromString(batman, event.popBack()->data<char>());


    socket::MutableBuffer_ptr int32numsBuf = endiansafe::popBackAndDecode<int32_t>(event);
    int32_t* int32nums = int32numsBuf->data<int32_t>();
    unsigned int32numsSize = int32numsBuf->numElements<int32_t>();

    socket::MutableBuffer_ptr uint16numsBuf = endiansafe::popBackAndDecode<uint16_t>(event);
    uint16_t* uint16nums = uint16numsBuf->data<uint16_t>();
    unsigned uint16numsSize = uint16numsBuf->numElements<uint16_t>();

    if(debug)
    {
        boost::lock_guard<boost::mutex> lock(streamLock);
        std::cout << "==================================" << std::endl
            << "NUM-event received!" << std::endl
            << "Event id:  " << event.eventid() << std::endl;

        std::cout << "People:" << std::endl
            << "   " << spiderman << std::endl
            << "   " << batman << std::endl;
        //printAddressBook(address_book);

        std::cout << "Numeric data:" << std::endl << "   ";
        for(int i = 0; i < uint16numsSize; i++)
        {
            std::cout << std::setw(8) << uint16nums[i] << " ";
        }
        std::cout << std::endl << "   ";
        for(int i = 0; i < int32numsSize; i++)
        {
            std::cout << std::setw(8) << int32nums[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "==================================" << std::endl;
    } else {
        boost::lock_guard<boost::mutex> lock(streamLock);
        static int counter = 0;
        if(counter%1000 == 0)
            std::cerr << ".";
        counter ++;
    }
    session->sendEvent(NUM_RESPONSE_ID, socket::ConstBuffer("OK", 3));
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
        boost::lock_guard<boost::mutex> lock(streamLock);
        std::cout << session->getRemoteEndpointAddress()
            << " disconnected" << std::endl;
    }
    sessionRegistry->remove(session);
}


int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        std::cerr << "Usage: " << argv[0]
            << " <host IP> <host port> [debug]" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);
    debug = argc == 4;

    socket::Facade_ptr facade = socket::Facade::make();
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(ECHO_ID, echoHandler);
    eventHandlerMap->addEventHandler(TOGGLE_DEBUG_EVENT_ID, toggleDebuggingHandler);
    eventHandlerMap->addEventHandler(NUM_ID, numHandler);
    facade->sessionRegistry()->setDisconnectHandler(boost::bind(disconnectHandler,
                                                                facade->sessionRegistry(),
                                                                _1));
    facade->setWorkerThreads(10, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));
    socket::Listener_ptr listener = facade->listen(socket::Address::make(host, port));
    facade->joinAllWorkerThreads();

    return 0;
}
