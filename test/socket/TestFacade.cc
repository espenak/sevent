#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE SocketFacade
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
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
    AllEventsHandler allEventsHandler(2);
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



BOOST_AUTO_TEST_SUITE_END()
