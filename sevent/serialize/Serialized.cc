#include "Serialized.h"

namespace sevent
{
    namespace serialize
    {
        Serialized::Serialized()
        {}

        Serialized::Serialized(unsigned int size_, char* data_) :
            size(size_), data(data_)
        {}

        Serialized::~Serialized()
        {
            delete[] data;
        }
    } // namespace serialize
} // namespace sevent
