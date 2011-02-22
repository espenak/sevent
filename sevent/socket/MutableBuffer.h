#pragma once
#include "size.h"


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
    } // namespace socket
} // namespace sevent
