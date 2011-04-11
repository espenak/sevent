#pragma once
#include <boost/any.hpp>
#include "Result.h"
#include "Pair.h"
#include "sevent/datastruct/SharedArray.h"

namespace sevent
{
    namespace serialize
    {
        template<typename T>
        class SharedArraySerializer
        {
            public:
                typedef T value_type;
                typedef datastruct::SharedArray<T> shared_array_type;
                typedef boost::shared_ptr<shared_array_type> shared_array_ptr;

            public:
                static serialize::BaseResult_ptr serialize(boost::any& data)
                {
                    shared_array_ptr a = boost::any_cast<shared_array_ptr>(data);
                    value_type* arr = a->array().get();
                    return serialize::ConstPtrResult::make(reinterpret_cast<const char*>(arr),
                                                           a->size()*sizeof(value_type));
                }

                static boost::any deserialize(datastruct::MutableCharArray_ptr serialized)
                {
                    value_type* data = reinterpret_cast<value_type*>(serialized->data);
                    shared_array_ptr arr = boost::make_shared<shared_array_type>(boost::shared_array<value_type>(data),
                                                                                 serialized->size/sizeof(value_type));
                    return arr;
                }
        };

        extern Pair CharSharedArray;
        extern Pair Int8SharedArray;
        extern Pair Uint8SharedArray;
        extern Pair Int16SharedArray;
        extern Pair Uint16SharedArray;
        extern Pair Int32SharedArray;
        extern Pair Uint32SharedArray;
    } // namespace serialize
} // namespace sevent
