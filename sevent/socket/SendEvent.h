#pragma once
#include <stdint.h>
#include "size.h"

namespace sevent {
    namespace socket {
        class SendEvent
        {
            public:
                typedef unsigned eventId_t;
                typedef uint32_t bufsize_t;
            public:
                SendEvent(eventId_t eventid, const char* data, bufsize_t dataSize);
                virtual ~SendEvent();
                eventId_t eventid() const;
                const char* data() const;
                bufsize_t dataSize() const;
            private:
                eventId_t _eventid;
                const char* _data;
                bufsize_t _dataSize;
        };
    } // namespace socket
} // namespace sevent
