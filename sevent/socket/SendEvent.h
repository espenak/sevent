#pragma once
#include <stdint.h>
#include "size.h"
#include "ConstBuffer.h"

namespace sevent {
    namespace socket {

        class AbstractSendEvent
        {
            public:
                typedef unsigned eventId_t;
            public:
                eventId_t eventid() const;
                AbstractSendEvent(eventId_t eventId);
                virtual ~AbstractSendEvent() {};
                virtual const ConstBuffer& at(unsigned index) const = 0;
                virtual unsigned size() const = 0;
            private:
                eventId_t _eventid;
        };

        //class SendEventVector
        //{
            //public:
                //typedef unsigned eventId_t;
            //public:
                //SendEvent(eventId_t eventid, ConstBufferVector buffers);
                //virtual ~SendEvent() {};
                //virtual const ConstBuffer& at(unsigned index) const;
                //virtual unsigned size() const;
            //private:
                //ConstBufferVector& _dataBuffers;
        //};

        class SendEvent : public AbstractSendEvent
        {
            public:
                SendEvent(eventId_t eventid, const char* data, bufsize_t bufSize);
                virtual const ConstBuffer& at(unsigned index) const;
                virtual unsigned size() const;
            private:
                ConstBuffer _const_buf;
        };
    } // namespace socket
} // namespace sevent
