#pragma once
#include "size.h"
#include <vector>


namespace sevent
{
    namespace socket
    {
        /** Const (unchangable) buffer. */
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
    } // namespace socket
} // namespace sevent
