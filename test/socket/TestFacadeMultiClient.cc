#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFacadeMultiClient
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "sevent/socket/Facade.h"
#include "sevent/event.h"
#include "sevent/serialize/String.h"
#include "helpers.h"

using namespace sevent::socket;
using namespace sevent;
using sevent::event::Buffer;
using sevent::event::Event;

typedef boost::shared_ptr<std::string> String_ptr;


boost::mutex assert_lock;
class LongStreamMultiClientEventsHandler : public CountingAllEventsHandler
{
    public:
        LongStreamMultiClientEventsHandler(int expectedCalls) :
            CountingAllEventsHandler(expectedCalls)
        {}

        virtual void doSomething(Facade_ptr facade,
                                 Session_ptr session,
                                 event::Event_ptr event)
        {
            String_ptr msg = event->first<String_ptr>(serialize::String);
            std::string expectedMessage("hello");
#ifdef SEVENT_USE_STRING_ID
            int evid = boost::lexical_cast<int>(event->eventid());
            expectedMessage += event->eventid();
#else
            int evid = event->eventid();
            expectedMessage += boost::lexical_cast<std::string>(event->eventid());
#endif
            // Need to lock since BOOST_REQUIRE_* is not threadsafe
            {
                boost::lock_guard<boost::mutex> lock(assert_lock);
                BOOST_REQUIRE_EQUAL(*msg, expectedMessage);

                BOOST_REQUIRE(evid >= 2000);
                BOOST_REQUIRE(evid <= 2004);
            }
        }
    private:
        std::string _expectedMessage;
};



void sendMessagesThread(Session_ptr session, int id, int messageCount,
                        std::string& msg, int sleep)
{
    if(sleep) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(sleep));
    }
    for(int i=0; i<messageCount; i++)
    {
#ifdef SEVENT_USE_STRING_ID
        std::string theid = boost::lexical_cast<std::string>(id);
#else
        int theid = id;
#endif
        session->sendEvent(Event::make(theid,
                                       Buffer::make(boost::make_shared<std::string>(msg),
                                                    serialize::String)));
    }
}

BOOST_FIXTURE_TEST_SUITE(MultiClientSuite, BasicFixture)

BOOST_AUTO_TEST_CASE( LongStreamMultiThreadMultiClient )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from 5 clients with 8 listening threads");
    int max = 10000;
    LongStreamMultiClientEventsHandler allEventsHandler(max);
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    boost::thread_group group;
    int clientCount = 5;
    Session_ptr* sessions = new Session_ptr[clientCount];
    for(int i = 0; i < clientCount; i++)
    {
        sessions[i] = facade->connect(listenAddr);
        std::string msg = std::string("hello") + boost::lexical_cast<std::string>(2000 + i);
        boost::thread* t = new boost::thread(
                sendMessagesThread, sessions[i], 2000 + i, max/clientCount, msg, 0);
        group.add_thread(t);
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}

BOOST_AUTO_TEST_CASE( LongStreamMultiThreadMultiClientMessWithOrder )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from 5 clients with 8 listening threads, and messing with the ordering");
    int max = 10000;
    LongStreamMultiClientEventsHandler allEventsHandler(max);
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    boost::thread_group group;
    int clientCount = 5;
    Session_ptr* sessions = new Session_ptr[clientCount];
    for(int i = 0; i < clientCount; i++)
    {
        sessions[i] = facade->connect(listenAddr);
    }
    boost::thread t0(sendMessagesThread, sessions[0], 2000, max/clientCount,
            std::string("hello2000"),
            500);

    boost::thread t1(sendMessagesThread, sessions[1], 2001, max/clientCount,
            std::string("hello2001"),
            20);

    boost::thread t2(sendMessagesThread, sessions[2], 2002, max/clientCount,
            std::string("hello2002"),
            100);

    boost::thread t3(sendMessagesThread, sessions[3], 2003, max/clientCount,
            std::string("hello2003"),
            0);

    boost::thread t4(sendMessagesThread, sessions[4], 2004, max/clientCount,
            std::string("hello2004"),
            0);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}

BOOST_AUTO_TEST_SUITE_END()
