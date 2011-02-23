#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StressTestSocketFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
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



boost::mutex assert_lock;
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
            sevent::socket::ReceiveEvent& event)
        {
            // Need to lock since BOOST_REQUIRE_* is not threadsafe
            {
                boost::lock_guard<boost::mutex> lock(assert_lock);
                BOOST_REQUIRE_EQUAL(event.dataSize(), _messageSize);
            }
            for(unsigned i = 0; i < _messageSize; i++)
            {
                boost::lock_guard<boost::mutex> lock(assert_lock);
                BOOST_REQUIRE_EQUAL(event.data<char*>()[i], 'x');
            }
        }
    private:
        unsigned _messageSize;
};


BOOST_FIXTURE_TEST_SUITE(StressSuite, StressFixture)

BOOST_AUTO_TEST_CASE( BigMessage )
{
    BOOST_TEST_MESSAGE("Testing sending a single 1mb message.");
    unsigned messageCount = 1;
    unsigned messageSize = OneMegaByte;
    unsigned workerThreadCount = 1;
    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount,
                             boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    sendSizedMessages(session, 2004, messageCount, messageSize);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}

BOOST_AUTO_TEST_CASE( BigMessages )
{
    BOOST_TEST_MESSAGE("Testing sending 5 1mb messages in a row.");
    unsigned messageCount = 5;
    unsigned messageSize = OneMegaByte;
    unsigned workerThreadCount = 1;
    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount,
                             boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    sendSizedMessages(session, 2004, messageCount, messageSize);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}

BOOST_AUTO_TEST_CASE( BigMessagesMultipleThreadsOnOneSession )
{
    // Tests if sendEvent is threadsafe
    BOOST_TEST_MESSAGE("Sending 20 0.5mb message from 10 threads _sharing the same session_.");
    unsigned messageCount = 20;
    unsigned messageSize = HalfMegaByte;
    unsigned threadCount = 10;
    unsigned workerThreadCount = 1;

    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount, boost::bind(
                                 boost::ref(allEventsHandler),
                                 _1, _2, _3));

    boost::thread_group group;
    BOOST_REQUIRE_EQUAL(messageCount%threadCount, 0);
    for(unsigned i = 0; i < threadCount; i++)
    {
        boost::thread* t = new boost::thread(sendSizedMessages, session,
                                             2004, messageCount/threadCount,
                                             messageSize);
        group.add_thread(t);
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}


BOOST_AUTO_TEST_CASE( BigMessagesMultipleThreadsOnMultipleSessions )
{
    BOOST_TEST_MESSAGE("Sending 20 0.5mb message from 10 sessions.");
    unsigned messageCount = 20;
    unsigned messageSize = HalfMegaByte;
    unsigned clientCount = 10;
    unsigned workerThreadCount = 1;

    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount, boost::bind(
                                 boost::ref(allEventsHandler),
                                 _1, _2, _3));

    boost::thread_group group;
    BOOST_REQUIRE_EQUAL(messageCount%clientCount, 0);
    Session_ptr* sessions = new Session_ptr[clientCount];
    for(unsigned i = 0; i < clientCount; i++)
    {
        sessions[i] = facade->connect(listenAddr);
        boost::thread* t = new boost::thread(sendSizedMessages, sessions[i],
                                             2004, messageCount/clientCount,
                                             messageSize);
        group.add_thread(t);
    }

    facade->joinAllWorkerThreads();
    delete[] sessions;
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}

BOOST_AUTO_TEST_CASE( BigMessagesThreaded )
{
    BOOST_TEST_MESSAGE("Sending 20 0.5mb message from 10 sessions.");
    unsigned messageCount = 20;
    unsigned messageSize = HalfMegaByte;
    unsigned clientCount = 10;
    unsigned workerThreadCount = 5;

    LongMessagesHandler allEventsHandler(messageCount, messageSize);
    facade->setWorkerThreads(workerThreadCount, boost::bind(
                                 boost::ref(allEventsHandler),
                                 _1, _2, _3));

    boost::thread_group group;
    BOOST_REQUIRE_EQUAL(messageCount%clientCount, 0);
    Session_ptr* sessions = new Session_ptr[clientCount];
    for(unsigned i = 0; i < clientCount; i++)
    {
        sessions[i] = facade->connect(listenAddr);
        boost::thread* t = new boost::thread(sendSizedMessages, sessions[i],
                                             2004, messageCount/clientCount,
                                             messageSize);
        group.add_thread(t);
    }

    facade->joinAllWorkerThreads();
    delete[] sessions;
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}


BOOST_AUTO_TEST_SUITE_END()
