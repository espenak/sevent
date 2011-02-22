#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/Facade.h"
#include "helpers.h"
#include <iostream>

using namespace sevent::socket;


BOOST_FIXTURE_TEST_SUITE(BasicSuite, BasicFixture)

BOOST_AUTO_TEST_CASE( Sanity )
{
    CountingAllEventsHandler allEventsHandler(2);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));
    session->sendEvent(1010, ConstBuffer("Hello", 6));
    session->sendEvent(2020, ConstBuffer("World", 6));
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
        LongStreamEventsHandler(int expectedCalls, std::string expectedMessage) :
            CountingAllEventsHandler(expectedCalls),
            _expectedMessage(expectedMessage)

        {}

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::socket::ReceiveEvent& event)
        {
            std::string msg(event.data());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage);
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.dataSize(), _expectedMessage.size()+1);
            //std::cout << msg << std::endl;
            delete[] event.data();
        }
    private:
        std::string _expectedMessage;
};


BOOST_AUTO_TEST_CASE( LongStreamSingleThread )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 1 listening thread");
    int max = 10000;
    LongStreamEventsHandler allEventsHandler(max, std::string("Hello world"));
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg("Hello world");
    for(int i=0; i<max; i++)
    {
        session->sendEvent(2020, ConstBuffer(msg.c_str(), msg.size()+1));
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}

BOOST_AUTO_TEST_CASE( LongStreamMultiThread )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 8 listening threads");
    int max = 10000;
    LongStreamEventsHandler allEventsHandler(max, std::string("Hello world"));
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg("Hello world");
    for(int i=0; i<max; i++)
    {
        session->sendEvent(2020, ConstBuffer(msg.c_str(), msg.size()+1));
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}


BOOST_AUTO_TEST_CASE( LongStreamBigMessageMultiThread )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 8 listening threads");
    int max = 10000;
    LongStreamEventsHandler allEventsHandler(max, std::string(10000, 'x'));
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg(10000, 'x');
    for(int i=0; i<max; i++)
    {
        session->sendEvent(2020, ConstBuffer(msg.c_str(), msg.size()+1));
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}


BOOST_AUTO_TEST_SUITE_END()
