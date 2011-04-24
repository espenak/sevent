#pragma once
#include <boost/any.hpp>
#include "Result.h"
#include "Pair.h"
#include "sevent/datastruct/SharedArray.h"
#include "EndianSafe.h"

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
                    unsigned size = a->size() * sizeof(value_type);
                    char* withEnd = new char[size + 1];  // Plus 1 for endianess
                    memcpy(withEnd, arr, size);
                    withEnd[size] = myEndianess();
                    return serialize::ConstPtrResult::make(
                                reinterpret_cast<const char*>(withEnd),
                                size + 1);
                }

                static boost::any deserialize(datastruct::MutableCharArray_ptr serialized)
                {
                    value_type* data = reinterpret_cast<value_type*>(serialized->data);
                    unsigned size = (serialized->size-1)/sizeof(value_type); // Minus 1 for endianess
                    shared_array_ptr arr = boost::make_shared<shared_array_type>(
                                                boost::shared_array<value_type>(data),
                                                size);
                    Endianess endianess = static_cast<Endianess>(serialized->data[serialized->size-1]);
                    if(endianess != myEndianess())
                    {
                        for(int i = 0; i < size; i++)
                        {
                            data[i] = endianSwap<value_type>(data[i]);
                        }
                    }
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
