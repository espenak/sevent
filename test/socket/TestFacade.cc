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
    CountingAllEventsHandler allEventsHandler(4);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));
    session->sendEvent(1010, ConstBuffer("Hello", 6));
    session->sendEvent(2020, ConstBuffer("Wor", 4));
    session->sendEvent(3030, ConstBuffer("Wo", 3));
    session->sendEvent(4040, ConstBuffer("W", 2));
    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 4);
}

BOOST_AUTO_TEST_CASE( SanityMultiBuffer )
{
    CountingAllEventsHandler allEventsHandler(3);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));
    ConstBufferVector v;
    v.push_back(ConstBuffer("Hel", 4));
    v.push_back(ConstBuffer("crue", 5));
    v.push_back(ConstBuffer("worlddd", 8));
    v.push_back(ConstBuffer("tu", 2));
    session->sendEvent(1010, v);

    ConstBufferVector v2;
    v2.push_back(ConstBuffer("Yo", 3));
    v2.push_back(ConstBuffer("dude", 5));
    session->sendEvent(2020, v2);

    ConstBufferVector v3;
    v3.push_back(ConstBuffer("T", 2));
    v3.push_back(ConstBuffer("es", 3));
    v3.push_back(ConstBuffer("t", 2));
    session->sendEvent(3030, v3);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 3);
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
            std::string msg(event.data<char*>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage);
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.dataSize(), _expectedMessage.size()+1);
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


class LongStreamMultibufEventsHandler : public CountingAllEventsHandler
{
    public:
        LongStreamMultibufEventsHandler(int expectedCalls, std::string expectedMessage) :
            CountingAllEventsHandler(expectedCalls),
            _expectedMessage(expectedMessage)

        {}

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::socket::ReceiveEvent& event)
        {
            std::string msg(event.data<char*>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage);
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.dataSize(), _expectedMessage.size()+1);
            //std::cout << msg << std::endl;
        }
    private:
        std::string _expectedMessage;
};






class LongMultibufStreamEventsHandler : public CountingAllEventsHandler
{
    public:
        LongMultibufStreamEventsHandler(int expectedCalls,
                                        std::string expectedMessage1,
                                        std::string expectedMessage2,
                                        std::string expectedMessage3) :
            CountingAllEventsHandler(expectedCalls),
            _expectedMessage1(expectedMessage1),
            _expectedMessage2(expectedMessage2),
            _expectedMessage3(expectedMessage3)
        {}

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::socket::ReceiveEvent& event)
        {
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);

            // It should work just like a "single message" event with the first
            // element
            std::string msg(event.data<char*>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage1);
            BOOST_REQUIRE_EQUAL(event.dataSize(), _expectedMessage1.size()+1);

            std::string msg1(event.datavector->at(0)->data<char*>());
            std::string msg2(event.datavector->at(1)->data<char*>());
            std::string msg3(event.datavector->at(2)->data<char*>());
            BOOST_REQUIRE_EQUAL(msg1, _expectedMessage1);
            BOOST_REQUIRE_EQUAL(msg2, _expectedMessage2);
            BOOST_REQUIRE_EQUAL(msg3, _expectedMessage3);
        }
    private:
        std::string _expectedMessage1;
        std::string _expectedMessage2;
        std::string _expectedMessage3;
};



BOOST_AUTO_TEST_CASE( LongStreamBigMessageMultiThreadMultiBuf )
{
    BOOST_TEST_MESSAGE("Testing with 10000 messages from ONE client with 8 listening threads multibuffer");
    int max = 10000;
    LongMultibufStreamEventsHandler allEventsHandler(max,
                                                     std::string(3000, 'x'),
                                                     std::string(9000, 'y'),
                                                     std::string(6000, 'z'));
    facade->setWorkerThreads(8,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    std::string msg1(3000, 'x');
    std::string msg2(9000, 'y');
    std::string msg3(6000, 'z');
    for(int i=0; i<max; i++)
    {
        ConstBufferVector v;
        v.push_back(ConstBuffer(msg1.c_str(), msg1.size()+1));
        v.push_back(ConstBuffer(msg2.c_str(), msg2.size()+1));
        v.push_back(ConstBuffer(msg3.c_str(), msg3.size()+1));
        session->sendEvent(2020, v);
    }

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), max);
}


BOOST_AUTO_TEST_SUITE_END()
