#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestExample
#include <boost/test/unit_test.hpp>
#include "sevent/sevent.h"

using namespace sevent;
using sevent::event::Event;

unsigned counter = 0;
void allEventsHandler(socket::Facade_ptr facade,
                      socket::Session_ptr session,
                      event::Event_ptr event) {
    counter ++;
    if(counter == 2) {
        facade->service()->stop();
    }
}

BOOST_AUTO_TEST_CASE(SimpleTestExample) {
    socket::Address_ptr listenAddr = socket::Address::make(
                                        "127.0.0.1", 9091);
    socket::Facade_ptr facade = socket::Facade::make();
    facade->setWorkerThreads(1, allEventsHandler);
    facade->listen(listenAddr);
    socket::Session_ptr session = facade->connect(listenAddr);

    facade->sendEvent(session, Event::make("ping"));
    facade->sendEvent(session, Event::make("ping"));

    facade->joinAllWorkerThreads();
    BOOST_REQUIRE_EQUAL(counter, 2);
}
