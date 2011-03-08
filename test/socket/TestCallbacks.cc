#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestCallbacks
#include <boost/test/unit_test.hpp>
#include <string>
#include "sevent/sevent.h"
using namespace sevent::socket;



void allEventsHandler(bool* called,
                      Facade_ptr facade,
                      Session_ptr session,
                      ReceiveEvent& event)
{
    *called = true;
}

void disconnectHandler(bool* called,
                       Facade_ptr facade,
                       Session_ptr session)
{
    facade->sessionRegistry()->remove(session);
    *called = true;
    facade->service()->stop();
}

void workerThreadHandler(bool* called,
                         Facade_ptr facade)
{
    facade->service()->run(); // loops until exception, or facade->service()->stop()
    *called = true;
}


BOOST_AUTO_TEST_CASE( TestCallbacksMain )
{
    bool allEventsHandlerCalled = false;
    bool disconnectHandlerCalled = false;
    bool workerThreadHandlerCalled = false;

    Facade_ptr serverFacade = Facade::make();

    serverFacade->setWorkerThreads(1,
                                   boost::bind(workerThreadHandler,
                                               &workerThreadHandlerCalled,
                                               _1),
                                   boost::bind(allEventsHandler,
                                               &allEventsHandlerCalled,
                                               _1, _2, _3));
    serverFacade->sessionRegistry()->setDisconnectHandler(boost::bind(disconnectHandler,
                                                                      &disconnectHandlerCalled,
                                                                      serverFacade,
                                                                      _1));

    Address_ptr listenAddr = Address::make("127.0.0.1", 9091);
    Listener_ptr listener = serverFacade->listen(listenAddr);

    Facade_ptr clientFacade = Facade::make();
    Session_ptr session = clientFacade->connect(listenAddr);
    session->sendEvent(101, ConstBuffer(0, 0));
    clientFacade->sessionRegistry()->remove(session); // Closes the session, which should make severFacade invoke it's disconnectHandler.

    serverFacade->joinAllWorkerThreads();
    BOOST_REQUIRE(allEventsHandlerCalled);
    BOOST_REQUIRE(disconnectHandlerCalled);
    BOOST_REQUIRE(workerThreadHandlerCalled);
}
