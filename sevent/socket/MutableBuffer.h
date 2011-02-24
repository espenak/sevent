#pragma once
#include "size.h"
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>


namespace sevent
{
    namespace socket
    {
        /** Mutable (changable/non-const) buffer.
         * The data is stored as a boost::shared_array, and will be deallocated
         * with the buffer unless you have another reference to it. */
        class MutableBuffer
        {
            public:
                MutableBuffer(boost::shared_array<char> data, bufsize_t size) :
                    _data(data), _size(size) {}
                virtual ~MutableBuffer(){}

                template<typename T>
                T data()
                {
                    return static_cast<T>(_data.get());
                }

                bufsize_t size()
                {
                    return _size;
                }
            private:
                boost::shared_array<char> _data;
                bufsize_t _size;
        };
        typedef boost::shared_ptr<MutableBuffer> MutableBuffer_ptr;
        typedef std::vector<MutableBuffer_ptr> MutableBufferVector;
        typedef boost::shared_ptr<MutableBufferVector> MutableBufferVector_ptr;
    } // namespace socket
} // namespace sevent
