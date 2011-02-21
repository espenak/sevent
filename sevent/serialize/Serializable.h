#pragma once
#include "Serialized.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace serialize
    {
        class Serializable
        {
            public:
                virtual ~Serializable() {};
                virtual Serialized_ptr serialize() = 0;
        };
        typedef boost::shared_ptr<Serializable> Serializable_ptr;
    } // namespace serialize
} // namespace sevent
