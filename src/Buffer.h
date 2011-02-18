#pragma once

namespace socketevent
{

class Buffer
{
    public:
        Buffer(char* data, unsigned size);
        virtual ~Buffer();
        char* data();
        unsigned size();
    private:
        char* _data;
        unsigned _size;
};

}
