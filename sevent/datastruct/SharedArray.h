#pragma once
#include <boost/shared_array.hpp>
#include <stdint.h>

namespace sevent
{
    namespace datastruct
    {
        template<typename T>
        class SharedArray
        {
            public:
                typedef T value_type;
                typedef boost::shared_array<value_type> value_shared_array;
                value_shared_array arr;
                unsigned size;
            public:
                SharedArray(value_shared_array sharedarr_, unsigned size_) :
                    arr(sharedarr_), size(size_) {}
        };

        typedef SharedArray<char> CharSharedArray;
        typedef boost::shared_ptr<CharSharedArray> CharSharedArray_ptr;

        typedef SharedArray<uint8_t> Uint8SharedArray;
        typedef boost::shared_ptr<Uint8SharedArray> Uint8SharedArray_ptr;
        typedef SharedArray<int8_t> Int8SharedArray;
        typedef boost::shared_ptr<Int8SharedArray> Int8SharedArray_ptr;

        typedef SharedArray<int16_t> Int16SharedArray;
        typedef boost::shared_ptr<Int16SharedArray> Int16SharedArray_ptr;
        typedef SharedArray<uint16_t> Uint16SharedArray;
        typedef boost::shared_ptr<Uint16SharedArray> Uint16SharedArray_ptr;

        typedef SharedArray<int32_t> Int32SharedArray;
        typedef boost::shared_ptr<Int32SharedArray> Int32SharedArray_ptr;
        typedef SharedArray<uint32_t> Uint32SharedArray;
        typedef boost::shared_ptr<Uint32SharedArray> Uint32SharedArray_ptr;
    } // namespace datastruct
} // namespace sevent
