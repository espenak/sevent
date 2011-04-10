#pragma once
#include "Result.h"
#include "Pair.h"

namespace sevent
{
    namespace serialize
    {
        serialize::BaseResult_ptr serializeString(boost::any& data);
        boost::any deserializeString(datastruct::MutableCharArray_ptr serialized);
        extern Pair String;
    } // namespace serialize
} // namespace sevent
