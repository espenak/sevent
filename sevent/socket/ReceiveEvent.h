#pragma once
#include <stdint.h>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include "Buffer.h"

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
                ReceiveEvent(eventId_t eventid, BufferBaseVector_ptr datavector_) :
                    _eventid(eventid), datavector(datavector_) {}

                virtual ~ReceiveEvent() {}

                eventId_t eventid() const
                {
                    return _eventid;
                }

                //template<typename T, typename SerializeCls>
                //boost::shared_ptr<T> popBack()
                //{
                    //BufferBase_ptr last = datavector->back();
                    //datavector->pop_back();
                    //return boost::dynamic_pointer_cast< Buffer<T, SerializeCls> >(last);
                //}

                template<typename T, typename SerializeCls>
                boost::shared_ptr<T> first()
                {
                    typedef Buffer<T, SerializeCls> Buffer_t;
                    BufferBase_ptr b = datavector->at(0);
                    boost::shared_ptr<Buffer_t> buffer = boost::static_pointer_cast<Buffer_t>(b);
                    boost::shared_ptr<T> data = buffer->data();
                    return data;
                }

            public:
                BufferBaseVector_ptr datavector;
            private:
                eventId_t _eventid;
                char* _data;
                dataSize_t _dataSize;
        };
    } // namespace socket
} // namespace sevent
