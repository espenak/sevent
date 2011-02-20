#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SocketFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/Facade.h"
#include "helpers.h"
#include <iostream>

using namespace sevent::socket;


struct BasicFixture {
    Facade_ptr facade;
    Listener_ptr listener;
    Session_ptr session;
    Address_ptr listenAddr;
    Address_ptr sessionAddr;

    BasicFixture ()
    {
        facade = Facade::make();
        listenAddr = Address::make("127.0.0.1", 9091);
        listener = facade->listen(listenAddr);
        session = facade->connect(listenAddr);
    }

    ~BasicFixture () {}
};



BOOST_FIXTURE_TEST_SUITE(BasicSuite, BasicFixture)

BOOST_AUTO_TEST_CASE( Sanity )
{
    CountingAllEventsHandler allEventsHandler(2);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));
    session->sendEvent(SendEvent(1010, "Hello", 6));
    session->sendEvent(SendEvent(2020, "World", 6));
    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 2);
}

BOOST_AUTO_TEST_CASE( ListenDuplicate )
{
    BOOST_REQUIRE_THROW(
            Listener_ptr listenerDup = facade->listen(Address::make("127.0.0.1", 9091)),
            std::runtime_error);
}

BOOST_AUTO_TEST_CASE( ConnectToInvalidPort )
{
    BOOST_REQUIRE_THROW(
            Session_ptr sessionDup = facade->connect(Address::make("127.0.0.1", 20000)),
            std::runtime_error);
}

//// Takes a long time, so commented out.. Could be used for debugging..
//BOOST_AUTO_TEST_CASE( ConnectToInvalidHost )
//{
    //BOOST_REQUIRE_THROW(
            //Session_ptr sessionDup = facade->connect(Address::make("127.0.0.10", 20000)),
            //std::runtime_error);
//}




class LongStreamEventsHandler : public CountingAllEventsHandler
{
    public:
        LongStreamEventsHandler(int expectedCalls)
            : CountingAllEventsHandler(expectedCalls)
        {
        }

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::socket::ReceiveEvent event)
        {
            std::string msg(event.data());
            BOOST_REQUIRE_EQUAL(msg, std::string("Hello world"));
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.dataSize(), 12);
            //std::cout << msg << std::endl;
        }
};


BOOST_AUTO_TEST_CASE( LongStreamSingleThread )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 1 listening thread");
    int max = 10000;
    LongStreamEventsHandler allEventsHandler(max);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg("Hello world");
    for(int i=0; i<max; i++)
    {
        session->sendEvent(SendEvent(2020, msg.c_str(), msg.size()+1));
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}

BOOST_AUTO_TEST_CASE( LongStreamMiltiThread )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 8 listening threads");
    int max = 10000;
    LongStreamEventsHandler allEventsHandler(max);
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg("Hello world");
    for(int i=0; i<max; i++)
    {
        session->sendEvent(SendEvent(2020, msg.c_str(), msg.size()+1));
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}









class LongStreamMultiClientEventsHandler : public CountingAllEventsHandler
{
    public:
        LongStreamMultiClientEventsHandler(int expectedCalls)
            : CountingAllEventsHandler(expectedCalls)
        {
        }

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::socket::ReceiveEvent event)
        {
            std::string msg(event.data());
            if(event.eventid() == 2001)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("hello"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 6);
            } else if(event.eventid() == 2002)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("world"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 6);
            } else if(event.eventid() == 2003)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("\n\r\b\t"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 5);
            } else if(event.eventid() == 2004)
            {
                int bufSize = 10000;
                BOOST_REQUIRE_EQUAL(event.dataSize(), bufSize);
                for(int i = 0; i < bufSize; i++)
                {
                    BOOST_REQUIRE_EQUAL(event.data()[i], 'x');
                    BOOST_MESSAGE( i );
                }
            } else if(event.eventid() == 2005)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("test"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 5);
            }
        }
};


void sendMessagesThread(Session_ptr session, int id, int messageCount, const std::string msg)
{
    for(int i=0; i<messageCount; i++)
    {
        session->sendEvent(SendEvent(id, msg.c_str(), msg.size()+1));
    }
}
void sendHugeMessagesThread(Session_ptr session, int id, int messageCount)
{
    int bufSize = 10000;
    char buf[bufSize];
    for(int i = 0; i < bufSize; i++)
    {
        buf[i] = 'x';
    }
    for(int i=0; i<messageCount; i++)
    {
        session->sendEvent(SendEvent(id, buf, bufSize));
    }
}

BOOST_AUTO_TEST_CASE( LongStreamMiltiThreadMultiClient )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from 5 clients with 8 listening threads");
    int max = 10000;
    LongStreamMultiClientEventsHandler allEventsHandler(max);
    facade->setWorkerThreads(5,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    boost::thread t1(sendMessagesThread, session, 2001, max/5, std::string("hello"));
    boost::thread t2(sendMessagesThread, session, 2002, max/5, std::string("world"));
    boost::thread t3(sendMessagesThread, session, 2003, max/5, std::string("\n\r\b\t"));
    //boost::thread t4(sendMessagesThread, session, 2004, max/5, std::string(10000, 's'));
    //boost::thread t4(sendMessagesThread, session, 2004, max/5, std::string("y"));
    boost::thread t4(sendHugeMessagesThread, session, 2004, max/5);
    boost::thread t5(sendMessagesThread, session, 2005, max/5, std::string("test"));

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}


BOOST_AUTO_TEST_SUITE_END()
