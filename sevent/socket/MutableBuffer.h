#pragma once
#include "size.h"
#include <vector>
#include <boost/shared_ptr.hpp>


namespace sevent
{
    namespace socket
    {
        class MutableBuffer
        {
            public:
                MutableBuffer(char* data, bufsize_t size) :
                    _data(data), _size(size) {}
                virtual ~MutableBuffer(){}

                template<typename T>
                T data() { return static_cast<T>(_data); }


                bufsize_t size() { return _size; }
            private:
                char* _data;
                bufsize_t _size;
        };
        typedef std::vector<MutableBuffer> MutableBufferVector;
        typedef boost::shared_ptr<MutableBufferVector> MutableBufferVector_ptr;
    } // namespace socket
} // namespace sevent
