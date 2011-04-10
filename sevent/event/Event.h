#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/thread/mutex.hpp>
#include "Buffer.h"
#include "EventId.h"

namespace sevent
{
    namespace event
    {
        class Event;
        typedef boost::shared_ptr<Event> Event_ptr;
        class Event
        {
            private:
                typedef std::vector<Buffer_ptr> BufferVector;

            public:
                static Event_ptr make(eventid_t::value_typeref eventid)
                {
                    return boost::make_shared<Event>(eventid);
                }
                static Event_ptr make(eventid_t::value_typeref eventid, Buffer_ptr first)
                {
                    return boost::make_shared<Event>(eventid, first);
                }
                static Event_ptr make(eventid_t::value_typeref eventid,
                                      datastruct::MutableCharArrayVector_ptr serialized)
                {
                    return boost::make_shared<Event>(eventid, serialized);
                }

            public:
                Event(eventid_t::value_typeref eventid) :
                    _eventid(eventid)
                {}

                Event(eventid_t::value_typeref eventid, Buffer_ptr first) :
                    _eventid(eventid)
                {
                    _buffers.push_back(first);
                }

                Event(eventid_t::value_typeref eventid, datastruct::MutableCharArrayVector_ptr serialized) :
                    _eventid(eventid), _buffers(serialized->size()), _serialized(serialized),
                    _isSerialized(serialized->size())
                {
                    for(int i = 0; i < serialized->size(); i++)
                    {
                        _isSerialized[i] = true;
                    }
                }

                template<typename T>
                T at(unsigned index, const serialize::Pair& serializer)
                {
                    return buffer_at(index, serializer)->data<T>();
                }

                template<typename T>
                T first(const serialize::Pair& serializer)
                {
                    return at<T>(0, serializer);
                }

                template<typename T>
                T back(const serialize::Pair& serializer)
                {
                    return at<T>(size()-1, serializer);
                }

                serialize::BaseResult_ptr serialize_at(int index)
                {
                    return _buffers.at(index)->serialize();
                }

                void push_back(Buffer_ptr buffer)
                {
                    boost::lock_guard<boost::mutex> lock(_lock);
                    _buffers.push_back(buffer);
                    _isSerialized.push_back(false);
                }

                void pop_back()
                {
                    _buffers.pop_back();
                }

                bool isSerialized(int index)
                {
                    return _serialized && _isSerialized[index];
                }

                unsigned size()
                {
                    return _buffers.size();
                }

                const eventid_t::value_typeref eventid()
                {
                    return _eventid.value();
                }

                void setEventid(eventid_t::value_typeref eventid)
                {
                    _eventid = eventid;
                }

                eventid_t& eventid_object()
                {
                    return _eventid;
                }

            private:
                void set_buffer(unsigned index, Buffer_ptr buffer)
                {
                    _buffers[index] = buffer;
                    _isSerialized[index] = false;
                }

                Buffer_ptr buffer_at(int index, const serialize::Pair& serializer)
                {
                    boost::lock_guard<boost::mutex> lock(_lock);
                    if(isSerialized(index))
                    {
                        datastruct::MutableCharArray_ptr mutarr = _serialized->at(index);
                        Buffer_ptr buffer = Buffer::deserialize(mutarr,
                                                                serializer);
                        mutarr.reset(); // Free the datastruct::MutableCharArray_ptr (data ownership is given to the deserializer)
                        set_buffer(index, buffer);
                    }
                    return _buffers.at(index);
                }

            private:
                eventid_t _eventid;
                BufferVector _buffers;
                datastruct::MutableCharArrayVector_ptr _serialized;
                boost::dynamic_bitset<> _isSerialized;
                boost::mutex _lock;
        };
    }
}
