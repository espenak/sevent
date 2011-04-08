#pragma once

#include <string>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "sevent/socket/Facade.h"

class CountingAllEventsHandler
{
    public:
        typedef void result_type;
    public:
        CountingAllEventsHandler(unsigned expectedCalls) :
            _counter(0), _expectedCalls(expectedCalls)
        {}

        void operator()(sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session, sevent::event::Event_ptr event)
        {
            doSomething(facade, session, event);
            boost::lock_guard<boost::mutex> lock(counter_lock);
            _counter ++;
            if (_counter == _expectedCalls)
            {
                facade->service()->stop();
            }
        }

        virtual void doSomething(
                sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session,
                sevent::event::Event_ptr event) {}

        unsigned counter() const
        {
            return _counter;
        }

    private:
        unsigned _counter;
        unsigned _expectedCalls;
        boost::mutex counter_lock;
};

enum Sizes { OneKiloByte=1024, HalfMegaByte=524288, OneMegaByte=1048576 };


struct BasicFixture {
    sevent::socket::Facade_ptr facade;
    sevent::socket::Listener_ptr listener;
    sevent::socket::Session_ptr session;
    sevent::socket::Address_ptr listenAddr;
    sevent::socket::Address_ptr sessionAddr;
    BasicFixture()
    {
        facade = sevent::socket::Facade::make();
        listenAddr = sevent::socket::Address::make("127.0.0.1", 9091);
        listener = facade->listen(listenAddr);
        session = facade->connect(listenAddr);
    }

    ~BasicFixture () {}
};

#ifdef SEVENT_USE_STRING_ID
    std::string eventid1("sevent::test::EventOne");
    std::string eventid2("sevent::test::EventTwo");
    std::string eventid3("sevent::test::EventThree");
    std::string eventid4("sevent::test::EventFour");
    std::string eventid5("sevent::test::EventFive");
#else
    uint32_t eventid1(1010);
    uint32_t eventid2(2020);
    uint32_t eventid3(3030);
    uint32_t eventid4(4040);
    uint32_t eventid5(5050);
#endif
