#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "sevent/socket/Facade.h"
#include "sevent/event.h"
#include "sevent/serialize/String.h"
#include "helpers.h"
#include <iostream>

using namespace sevent::socket;
using namespace sevent;
using sevent::event::Buffer;
using sevent::event::Event;

typedef boost::shared_ptr<std::string> String_ptr;

BOOST_FIXTURE_TEST_SUITE(BasicSuite, BasicFixture)


BOOST_AUTO_TEST_CASE( Sanity )
{
    CountingAllEventsHandler allEventsHandler(5);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));

    String_ptr hello = boost::make_shared<std::string>("Hello");
    String_ptr cruel = boost::make_shared<std::string>("supercruel");
    String_ptr world = boost::make_shared<std::string>("world!");

    session->sendEvent(Event::make(1010));
    session->sendEvent(Event::make(2020,
                                   Buffer::make(hello, serialize::String)));
    session->sendEvent(Event::make(3030,
                                   Buffer::make(cruel, serialize::String)));
    session->sendEvent(Event::make(4040,
                                   Buffer::make(world, serialize::String)));
    event::Event_ptr event = Event::make(5050);
    event->push_back(Buffer::make(hello, serialize::String));
    event->push_back(Buffer::make(cruel, serialize::String));
    event->push_back(Buffer::make(world, serialize::String));
    session->sendEvent(event);

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 5);
}


/*

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



void stoppingDisconnectHandler(Facade_ptr facade,
                               Session_ptr session)
{
    facade->sessionRegistry()->remove(session);
    facade->service()->stop();
}
BOOST_AUTO_TEST_CASE( Disconnect )
{
    CountingAllEventsHandler allEventsHandler(1);
    facade->setWorkerThreads(1, boost::bind(boost::ref(allEventsHandler),
                                            _1, _2, _3));
    facade->sessionRegistry()->setDisconnectHandler(boost::bind(stoppingDisconnectHandler,
                                                                facade,
                                                                _1));
    facade->sessionRegistry()->remove(session);
    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 0);
}




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
            std::string msg(event.first()->data<char>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage);
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.first()->size(), _expectedMessage.size()+1);
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
            std::string msg(event.first()->data<char>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage);
            BOOST_REQUIRE_EQUAL(event.eventid(), 2020);
            BOOST_REQUIRE_EQUAL(event.first()->size(), _expectedMessage.size()+1);
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
            std::string msg(event.first()->data<char>());
            BOOST_REQUIRE_EQUAL(msg, _expectedMessage1);
            BOOST_REQUIRE_EQUAL(event.first()->size(), _expectedMessage1.size()+1);

            std::string msg1(event.datavector->at(0)->data<char>());
            std::string msg2(event.datavector->at(1)->data<char>());
            std::string msg3(event.datavector->at(2)->data<char>());
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
*/


BOOST_AUTO_TEST_SUITE_END()
