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
                    value_type* arr = a->arr.get();
                    return serialize::ConstPtrResult::make(reinterpret_cast<const char*>(arr),
                                                           a->size*sizeof(value_type));
                }

                static boost::any deserialize(datastruct::MutableCharArray_ptr serialized)
                {
                    value_type* data = reinterpret_cast<value_type*>(serialized->data);
                    shared_array_ptr arr = boost::make_shared<shared_array_type>(boost::shared_array<value_type>(data),
                                                                                 serialized->size/sizeof(value_type));
                    return arr;
                }
        };

        Pair CharSharedArray(SharedArraySerializer<char>::serialize,
                             SharedArraySerializer<char>::deserialize);

        Pair Int8SharedArray(SharedArraySerializer<int8_t>::serialize,
                               SharedArraySerializer<int8_t>::deserialize);
        Pair Uint8SharedArray(SharedArraySerializer<uint8_t>::serialize,
                               SharedArraySerializer<uint8_t>::deserialize);

        Pair Int16SharedArray(SharedArraySerializer<int16_t>::serialize,
                               SharedArraySerializer<int16_t>::deserialize);
        Pair Uint16SharedArray(SharedArraySerializer<uint16_t>::serialize,
                               SharedArraySerializer<uint16_t>::deserialize);

        Pair Int32SharedArray(SharedArraySerializer<int32_t>::serialize,
                               SharedArraySerializer<int32_t>::deserialize);
        Pair Uint32SharedArray(SharedArraySerializer<uint32_t>::serialize,
                               SharedArraySerializer<uint32_t>::deserialize);
    } // namespace serialize
} // namespace sevent
