#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"
#include "Shared.h"
#include "BoostSerializedAddressBook.h"


using namespace sevent;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
bool debug = false;

int expectedResponseCount;
boost::mutex expectedResponseCountLock;
void quitIfFinished(socket::Facade_ptr facade,
                    socket::Session_ptr session)
{
    static int responseCount = 0;
    boost::lock_guard<boost::mutex> lock(expectedResponseCountLock);
    responseCount ++;
    if(responseCount == expectedResponseCount)
    {
        facade->service()->stop();
    }
}


void echoResponseHandler(socket::Facade_ptr facade,
                         socket::Session_ptr session,
                         socket::ReceiveEvent& event)
{
    char* data = event.first()->data<char>();
    if(debug)
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cerr << "ECHO-response-event received!" << std::endl
            << "      " << data << std::endl;
    }
    quitIfFinished(facade, session);
}

void numResponseHandler(socket::Facade_ptr facade,
                         socket::Session_ptr session,
                         socket::ReceiveEvent& event)
{
    char* data = event.first()->data<char>();
    if(debug)
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cerr << "Num-response-event received!" << std::endl
            << "      " << data << std::endl;
    }
    quitIfFinished(facade, session);
}

void allEventsHandler(event::HandlerMap_ptr eventHandlerMap,
                      socket::Facade_ptr facade,
                      socket::Session_ptr session,
                      socket::ReceiveEvent& event)
{
    eventHandlerMap->triggerEvent(facade, session, event);
}



void sendMessage(socket::Session_ptr session)
{
    // Lets send a couple of events! Note that the received order is not
    // guaranteed.
    
    session->sendEvent(ECHO_ID, socket::ConstBuffer("ECHO", 6));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("(somewhat) cruel", 17));
    session->sendEvent(ECHO_ID, socket::ConstBuffer("World", 6));

    // An endian-safe message containing arrays.
    {
        uint16_t uint16nums[] = {10, 20, 30, 40, 50, 60};
        int32_t int32nums[] = {-5000000, 50, -50, 5000, -5000};

        // Note that we do not use boostserialize::toConstBuffer, because the
        // string would go out of scope before the vector is sent!
        std::string spiderman = boostserialize::toString(Person(10, "Spiderman", "spider@man.com"));
        std::string batman = boostserialize::toString(Person(10, "Batman", "bat@man.com"));

        socket::ConstBufferVector vec;
        vec.push_back(socket::ConstBuffer(spiderman));
        endiansafe::pushBack(vec, uint16nums, 6);
        endiansafe::pushBack(vec, int32nums, 5);
        vec.push_back(socket::ConstBuffer(batman));
        session->sendEvent(NUM_ID, vec);
    }
}


int main(int argc, const char *argv[])
{
    if(argc < 4)
    {
        std::cerr << "Usage: " << argv[0]
            << " <host IP> <host port> <numMessages> [debug]" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);
    unsigned numMessages = boost::lexical_cast<unsigned>(argv[3]);
    debug = argc == 5;


    socket::Facade_ptr facade = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler(ECHO_RESPONSE_ID, echoResponseHandler);
    eventHandlerMap->addEventHandler(NUM_RESPONSE_ID, numResponseHandler);

    // Start 5 worker threads, and use the handler above for incoming events.
    facade->setWorkerThreads(5, boost::bind(allEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));

    // Make a session and show both sides of the communication
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cerr << "Connected to " << session->getRemoteEndpointAddress()
            << " as " << session->getLocalEndpointAddress() << std::endl;
    }

    for(int i = 0; i < numMessages; i++)
    {
        if(debug)
        {
            boost::lock_guard<boost::mutex> lock(stream_lock);
            std::cerr << "Sending message " << i << std::endl;
        }
        sendMessage(session);
        {
            boost::lock_guard<boost::mutex> lock(expectedResponseCountLock);
            expectedResponseCount += 4;
        }
    }
    facade->joinAllWorkerThreads();

    return 0;
}
