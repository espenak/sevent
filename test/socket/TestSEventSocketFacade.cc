#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestSEventSocketFacade
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



BOOST_AUTO_TEST_CASE( TestSEventSocketFacadeSanity )
{
    Facade_ptr facade = Facade::make();
    AllEventsHandler allEventsHandler(2);
    facade->setWorkerThreads(1,
            boost::bind(boost::ref(allEventsHandler), _1, _2, _3));
    Listener_ptr listener = facade->listen(Address::make("127.0.0.1", "9091"));
    Session_ptr session = facade->connect(Address::make("127.0.0.1", "9091"));

    char hello[6] = { 'h', 'e', 'l', 'l', 'o', '\0' };
    session->sendEvent(EventData(10, hello, 6));
    char die[6] = { 'w', 'o', 'r', 'l', 'd', '\0' };
    session->sendEvent(EventData(20, die, 6));

    facade->joinAllWorkerThreads();
    
    BOOST_REQUIRE_EQUAL(allEventsHandler.counter(), 2);
}
