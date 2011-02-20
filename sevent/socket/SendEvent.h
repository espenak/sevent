#pragma once
#include <stdint.h>

namespace sevent {
    namespace socket {
        class SendEvent
        {
            public:
                typedef unsigned eventId_t;
                typedef uint32_t dataSize_t;
            public:
                SendEvent(eventId_t eventid, const char* data, dataSize_t dataSize);
                virtual ~SendEvent();
                eventId_t eventid() const;
                const char* data() const;
                dataSize_t dataSize() const;
            private:
                eventId_t _eventid;
                const char* _data;
                dataSize_t _dataSize;
        };
    } // namespace socket
} // namespace sevent
