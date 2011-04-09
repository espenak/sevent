#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StressTestSocketFacade
#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "sevent/socket/Facade.h"
#include "sevent/event.h"
#include "sevent/serialize/String.h"
#include "helpers.h"

using namespace sevent::socket;
using namespace sevent;
using sevent::event::Buffer;
using sevent::event::Event;

typedef boost::shared_ptr<std::string> String_ptr;


struct StressFixture
{
    Facade_ptr facade;
    Session_ptr session;
    Address_ptr listenAddr;
    Address_ptr sessionAddr;

    StressFixture ()
    {
        facade = Facade::make();
        listenAddr = Address::make("127.0.0.1", 9091);
        facade->listen(listenAddr);
        session = facade->connect(listenAddr);
    }

    ~StressFixture () {
        facade->removeListener(Address("127.0.0.1", 9091));
    }
};


void sendSizedMessages(Session_ptr session, unsigned messageCount, unsigned messageSize)
{
    String_ptr msg = boost::make_shared<std::string>(messageSize, 'x');
    for(unsigned i=0; i<messageCount; i++)
    {
        session->sendEvent(Event::make(eventid1,
                                       Buffer::make(msg, serialize::String)));
    }
}



boost::mutex assert_lock;
class LongMessagesHandler : public CountingAllEventsHandler
{
    public:
        LongMessagesHandler(unsigned expectedCalls, unsigned messageSize) :
            CountingAllEventsHandler(expectedCalls),
            _messageSize(messageSize)
        {
        }

        virtual void doSomething(Facade_ptr facade,
                                 Session_ptr session,
                                 event::Event_ptr event)
        {
            // Need to lock since BOOST_REQUIRE_* is not threadsafe
            String_ptr msg = event->first<String_ptr>(serialize::String);
            {
                boost::lock_guard<boost::mutex> lock(assert_lock);
                BOOST_REQUIRE_EQUAL(msg->size(), _messageSize);
            }
            for(unsigned i = 0; i < _messageSize; i++)
            {
                boost::lock_guard<boost::mutex> lock(assert_lock);
                BOOST_REQUIRE_EQUAL((*msg)[i], 'x');
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

    sendSizedMessages(session, messageCount, messageSize);

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

    sendSizedMessages(session, messageCount, messageSize);

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
                                             messageCount/threadCount,
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
                                             messageCount/clientCount,
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
                                             messageCount/clientCount,
                                             messageSize);
        group.add_thread(t);
    }

    facade->joinAllWorkerThreads();
    delete[] sessions;
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), messageCount);
}


BOOST_AUTO_TEST_SUITE_END()
