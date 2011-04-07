#pragma once
#include <boost/any.hpp>
#include <boost/function.hpp>
#include "Result.h"
#include "sevent/datastruct/MutableCharArray.h"

namespace sevent
{
    namespace serialize
    {
        typedef boost::function<BaseResult_ptr (boost::any& data)> serialize_t;
        typedef boost::function<boost::any (datastruct::MutableCharArray_ptr serialized)> deserialize_t;
        struct Pair
        {
            serialize_t serializeFunc;
            deserialize_t deserializeFunc;
            Pair(serialize_t serializeFunc_, deserialize_t deserializeFunc_) :
                serializeFunc(serializeFunc_), deserializeFunc(deserializeFunc_) {}
        };
    }
}
