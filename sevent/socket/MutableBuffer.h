#pragma once
#include "size.h"
#include <boost/shared_ptr.hpp>


namespace sevent
{
    namespace socket
    {
        class MutableBuffer
        {
            public:
                MutableBuffer(void* data, bufsize_t size) :
                    _data(data), _size(size) {}
                virtual ~MutableBuffer() {}
                void* data() { return _data; }
                bufsize_t size() { return _size; }
            private:
                void* _data;
                bufsize_t _size;
        };
        typedef std::vector<MutableBuffer> MutableBufferVector;
        typedef boost::shared_ptr<MutableBufferVector> MutableBufferVector_ptr;
    } // namespace socket
} // namespace sevent
