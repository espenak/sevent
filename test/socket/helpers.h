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
                sevent::socket::Session_ptr session, sevent::socket::ReceiveEvent event)
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
                sevent::socket::ReceiveEvent event) {}

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

    BasicFixture ()
    {
        facade = sevent::socket::Facade::make();
        listenAddr = sevent::socket::Address::make("127.0.0.1", 9091);
        listener = facade->listen(listenAddr);
        session = facade->connect(listenAddr);
    }

    ~BasicFixture () {}
};


void sendSizedMessages(sevent::socket::Session_ptr session, unsigned id,
        unsigned messageCount, unsigned messageSize)
{
    char *buf = new char[messageSize];
    for(unsigned i = 0; i < messageSize; i++)
    {
        buf[i] = 'x';
    }
    for(unsigned i=0; i<messageCount; i++)
    {
        session->sendEvent(sevent::socket::SendEvent(id, buf, messageSize));
    }
    delete[] buf;
}


void sendStringMessagesThread(sevent::socket::Session_ptr session, unsigned id,
        unsigned messageCount, const std::string msg)
{
    for(unsigned i=0; i<messageCount; i++)
    {
        session->sendEvent(sevent::socket::SendEvent(id, msg.c_str(), msg.size()+1));
    }
}
