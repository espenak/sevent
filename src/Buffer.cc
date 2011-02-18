#include "Buffer.h"

namespace socketevent
{

Buffer::Buffer(char* data, unsigned size) : _data(data), _size(size)
{
}

Buffer::~Buffer()
{
}

unsigned Buffer::size()
{
    return _size;
}

char *Buffer::data()
{
    return _data;
}

}
