#pragma once
#include "size.h"
#include <vector>
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        class ConstBuffer
        {
            public:
                ConstBuffer(const void* data, bufsize_t size) :
                    _data(data), _size(size) {}
                virtual ~ConstBuffer() {}
                const void* data() const { return _data; }
                bufsize_t size() const { return _size; }
            private:
                const void* _data;
                bufsize_t _size;
        };

        typedef std::vector<ConstBuffer> ConstBufferVector;
        typedef boost::shared_ptr<ConstBufferVector> ConstBufferVector_ptr;
    } // namespace socket
} // namespace sevent
