#pragma once

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "sevent/socket/Facade.h"

class AllEventsHandler
{
    public:
        typedef void result_type;
    public:
        AllEventsHandler(int expectedCalls) :
            _counter(0), _expectedCalls(expectedCalls)
        {}

        void operator()(sevent::socket::Facade_ptr facade,
                sevent::socket::Session_ptr session, sevent::socket::ReceiveEvent event)
        {
            boost::lock_guard<boost::mutex> lock(counter_lock);
            _counter ++;
            if (_counter == _expectedCalls)
            {
                facade->service()->stop();
            }
        }

        int counter() const
        {
            return _counter;
        }

    private:
        int _counter;
        int _expectedCalls;
        boost::mutex counter_lock;
};
