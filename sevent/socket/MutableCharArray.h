#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>

namespace sevent
{
    namespace socket
    {
        struct MutableCharArray
        {
            char* data;
            unsigned size;
            MutableCharArray(char* data_, unsigned size_) : data(data_), size(size_) {}
        };
        typedef boost::shared_ptr<MutableCharArray> MutableCharArray_ptr;
        typedef std::vector<MutableCharArray_ptr> MutableCharArrayVector;
        typedef boost::shared_ptr<MutableCharArrayVector> MutableCharArrayVector_ptr;
    } // namespace socket
} // namespace sevent
