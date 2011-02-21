#pragma once
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace serialize
    {
        struct Serialized
        {
            unsigned size;
            char* data;
            Serialized();
            Serialized(unsigned int size, char* data);
            ~Serialized();
        };
        typedef boost::shared_ptr<Serialized> Serialized_ptr;
    } // namespace serialize
} // namespace sevent
