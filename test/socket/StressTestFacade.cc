#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StressTestSocketFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/Facade.h"
#include "helpers.h"
#include <iostream>

using namespace sevent::socket;


struct StressFixture
{
    Facade_ptr facade;
    Listener_ptr listener;
    Session_ptr session;
    Address_ptr listenAddr;
    Address_ptr sessionAddr;

    StressFixture ()
    {
        facade = Facade::make();
        listenAddr = Address::make("127.0.0.1", 9091);
        listener = facade->listen(listenAddr);
        session = facade->connect(listenAddr);
    }

    ~StressFixture () {}
};



class MultiEventsHandler : public CountingAllEventsHandler
{
    public:
        MultiEventsHandler(unsigned expectedCalls)
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
            }
            else if(event.eventid() == 2002)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("world"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 6);
            }
            else if(event.eventid() == 2003)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("\n\r\b\t"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 5);
            }
            else if(event.eventid() == 2004)
            {
                unsigned bufSize = 10000;
                BOOST_REQUIRE_EQUAL(event.dataSize(), bufSize);
                for(unsigned i = 0; i < bufSize; i++)
                {
                    BOOST_REQUIRE_EQUAL(event.data()[i], 'x');
                }
            }
            else if(event.eventid() == 2005)
            {
                BOOST_REQUIRE_EQUAL(msg, std::string("test"));
                BOOST_REQUIRE_EQUAL(event.dataSize(), 5);
            }
        }
};

class LongMessagesHandler : public CountingAllEventsHandler
{
    public:
        LongMessagesHandler(unsigned expectedCalls, unsigned messageSize) :
            CountingAllEventsHandler(expectedCalls),
            _messageSize(messageSize)
        {
        }

        virtual void doSomething(
            sevent::socket::Facade_ptr facade,
            sevent::socket::Session_ptr session,
            sevent::socket::ReceiveEvent event)
        {
            BOOST_REQUIRE_EQUAL(event.dataSize(), _messageSize);
            for(unsigned i = 0; i < _messageSize; i++)
            {
                BOOST_REQUIRE_EQUAL(event.data()[i], 'x');
            }
        }
    private:
        unsigned _messageSize;
};


void sendMessagesThread(Session_ptr session, unsigned id, unsigned messageCount, const std::string msg)
{
    for(unsigned i=0; i<messageCount; i++)
    {
        session->sendEvent(SendEvent(id, msg.c_str(), msg.size()+1));
    }
}
void sendHugeMessages(Session_ptr session, unsigned id, unsigned messageCount, unsigned messageSize)
{
    char buf[messageSize];
    //char *buf = new char[messageSize];
    for(unsigned i = 0; i < messageSize; i++)
    {
        buf[i] = 'x';
    }
    for(unsigned i=0; i<messageCount; i++)
    {
        session->sendEvent(SendEvent(id, buf, messageSize));
    }
}

BOOST_FIXTURE_TEST_SUITE(StressSuite, StressFixture)

BOOST_AUTO_TEST_CASE( HugeMessage )
{
    BOOST_TEST_MESSAGE("Testing sending a single 1G message.");
    unsigned messageCount = 1;
    //unsigned messageSize = 1000000000;
    unsigned messageSize = 1000000;
    unsigned workerThreadCount = 1;
    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount,
                             boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    sendHugeMessages(session, 2004, messageCount, messageSize);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}

//BOOST_AUTO_TEST_CASE( LongStreamHugeMessages )
//{
    //BOOST_TEST_MESSAGE("Testing sending multiple 1G messages.");
    //unsigned messageCount = 1;
    ////unsigned messageSize = 1000000000;
    //unsigned messageSize = 1000;
    //unsigned workerThreadCount = 1;
    //LongMessagesHandler allEventsHandler(messageCount, messageSize);
    //facade->setWorkerThreads(workerThreadCount,
                             //boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    //sendHugeMessages(session, 2004, messageCount, messageSize);

    //facade->joinAllWorkerThreads();
    //BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
//}

//BOOST_AUTO_TEST_CASE( LongStreamLongMessagesMultiClientAndThread )
//{
    //BOOST_TEST_MESSAGE("Testing with 10000 big messages (10000 chars) from 10 clients with 8 listening threads. This might take a couple of minutes.");
    //unsigned messageCount = 1000;
    //unsigned messageSize = 100000;
    //unsigned clientCount = 1;
    //unsigned workerThreadCount = 1;

    //LongMessagesHandler allEventsHandler(messageCount, messageSize);
    //facade->setWorkerThreads(workerThreadCount, boost::bind(
                                 //boost::ref(allEventsHandler),
                                 //_1, _2, _3));

    //boost::thread_group group;
    //BOOST_REQUIRE_EQUAL(messageCount%clientCount, 0);
    //for(unsigned i = 0; i < clientCount; i++)
    //{
        //boost::thread* t = new boost::thread(sendHugeMessages, session,
                                             //2004, messageCount/clientCount,
                                             //messageSize);
        //group.add_thread(t);
    //}

    //facade->joinAllWorkerThreads();
    //BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
//}

//BOOST_AUTO_TEST_CASE( LongStreamMiltiThreadMultiClient )
//{
//BOOST_TEST_MESSAGE("Testing with 10000 messages from 5 clients with 8 listening threads");
//unsigned messageCount = 10000;
//MultiEventsHandler allEventsHandler(messageCount);
//facade->setWorkerThreads(5,
//boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

//boost::thread t1(sendMessagesThread, session, 2001, messageCount/5, std::string("hello"));
//boost::thread t2(sendMessagesThread, session, 2002, messageCount/5, std::string("world"));
//boost::thread t3(sendMessagesThread, session, 2003, messageCount/5, std::string("\n\r\b\t"));
//boost::thread t4(sendHugeMessages, session, 2004, messageCount/5);
//boost::thread t5(sendMessagesThread, session, 2005, messageCount/5, std::string("test"));

//facade->joinAllWorkerThreads();
//BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
//}


BOOST_AUTO_TEST_SUITE_END()
