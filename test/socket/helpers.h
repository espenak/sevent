#pragma once

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "sevent/socket/Facade.h"

class CountingAllEventsHandler
{
    public:
        typedef void result_type;
    public:
        CountingAllEventsHandler(int expectedCalls) :
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

        int counter() const
        {
            return _counter;
        }

    private:
        int _counter;
        int _expectedCalls;
        boost::mutex counter_lock;
};
