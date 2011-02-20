#pragma once
#include <stdint.h>

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
                ReceiveEvent(eventId_t eventid, char* data, dataSize_t dataSize);
                virtual ~ReceiveEvent();
                eventId_t eventid();
                char* data();
                dataSize_t dataSize();
            private:
                eventId_t _eventid;
                char* _data;
                dataSize_t _dataSize;
        };

    } // namespace socket
} // namespace sevent
