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
                static Buffer_ptr make(boost::any anydata,
                                       const SerializePair& serializer)
                {
                    return boost::make_shared<Buffer>(anydata, serializer);
                }

                static Buffer_ptr deserialize(MutableCharArray_ptr serialized,
                                              const SerializePair& serializer)
                {
                    boost::any anydata = serializer.deserializeFunc(serialized);
                    return Buffer::make(anydata, serializer);
                }

            public:
                Buffer(boost::any anydata, const SerializePair& serializer) :
                    _anydata(anydata), _serializer(serializer) {}

                BaseSerializeResult_ptr serialize()
                {
                    return _serializer.serializeFunc(_anydata);
                }

                template<typename T> T data()
                {
                    return boost::any_cast<T>(_anydata);
                }

            private:
                boost::any _anydata;
                SerializePair _serializer;
        };
    } // namespace socket
} // namespace sevent
