#pragma once
#include <stdint.h>
#include "MutableBuffer.h"

namespace sevent
{
    namespace socket
    {

        class ReceiveEvent
        {
            public:
                typedef unsigned eventId_t;
                typedef uint32_t dataSize_t;
            public:
                ReceiveEvent(eventId_t eventid, MutableBufferVector_ptr datavector_);
                virtual ~ReceiveEvent();
                eventId_t eventid();
                MutableBuffer_ptr popBack();
                MutableBuffer_ptr first();
            public:
                MutableBufferVector_ptr datavector;
            private:
                eventId_t _eventid;
                char* _data;
                dataSize_t _dataSize;
        };

    } // namespace socket
} // namespace sevent
