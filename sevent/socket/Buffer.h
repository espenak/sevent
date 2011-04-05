#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include "SerializePair.h"


namespace sevent
{
    namespace socket
    {
        class Buffer;
        typedef boost::shared_ptr<Buffer> Buffer_ptr;
        class Buffer
        {
            public:
                static Buffer_ptr make(boost::any anydata, serialize_t serializeFunc)
                {
                    return boost::make_shared<Buffer>(anydata, serializeFunc);
                }

                static Buffer_ptr deserialize(MutableCharArray_ptr serialized,
                                              serialize_t serializeFunc,
                                              deserialize_t deserializeFunc)
                {
                    boost::any anydata = deserializeFunc(serialized);
                    return Buffer::make(anydata, serializeFunc);
                }

            public:
                Buffer(boost::any anydata, serialize_t serializeFunc) :
                    _anydata(anydata), _serializeFunc(serializeFunc) {}

                BaseSerializeResult_ptr serialize()
                {
                    return _serializeFunc(_anydata);
                }

                template<typename T> T data()
                {
                    return boost::any_cast<T>(_anydata);
                }

            private:
                boost::any _anydata;
                serialize_t _serializeFunc;
        };
    } // namespace socket
} // namespace sevent
