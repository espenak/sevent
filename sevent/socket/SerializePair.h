#pragma once
#include <boost/function.hpp>
#include "SerializeResult.h"
#include "MutableCharArray.h"

namespace sevent
{
    namespace socket
    {
        typedef boost::function<BaseSerializeResult_ptr (boost::any& data)> serialize_t;
        typedef boost::function<boost::any (MutableCharArray_ptr serialized)> deserialize_t;
        struct SerializePair
        {
            serialize_t serializeFunc;
            deserialize_t deserializeFunc;
            SerializePair(serialize_t serializeFunc_, deserialize_t deserializeFunc_) :
                serializeFunc(serializeFunc_), deserializeFunc(deserializeFunc_) {}
        };

    } // namespace socket
} // namespace sevent
