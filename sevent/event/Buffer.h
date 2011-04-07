#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include "sevent/serialize/Pair.h"


namespace sevent
{
    namespace event
    {
        class Buffer;
        typedef boost::shared_ptr<Buffer> Buffer_ptr;
        class Buffer
        {
            public:
                static Buffer_ptr make(boost::any anydata,
                                       const serialize::Pair& serializer)
                {
                    return boost::make_shared<Buffer>(anydata, serializer);
                }

                static Buffer_ptr deserialize(datastruct::MutableCharArray_ptr serialized,
                                              const serialize::Pair& serializer)
                {
                    boost::any anydata = serializer.deserializeFunc(serialized);
                    return Buffer::make(anydata, serializer);
                }

            public:
                Buffer(boost::any anydata, const serialize::Pair& serializer) :
                    _anydata(anydata), _serializer(serializer) {}

                serialize::BaseResult_ptr serialize()
                {
                    return _serializer.serializeFunc(_anydata);
                }

                template<typename T> T data()
                {
                    return boost::any_cast<T>(_anydata);
                }

            private:
                boost::any _anydata;
                serialize::Pair _serializer;
        };
    }
}
