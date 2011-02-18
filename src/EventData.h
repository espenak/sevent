#pragma once
#include <stdint.h>

namespace socketevent
{

class EventData
{
    public:
        typedef unsigned eventId_t;
        typedef uint32_t dataSize_t;
    public:
        EventData(eventId_t eventid, char* data, dataSize_t dataSize);
        virtual ~EventData();
        eventId_t eventid();
        char* data();
        dataSize_t dataSize();
    private:
        eventId_t _eventid;
        char* _data;
        dataSize_t _dataSize;
};

}
