#pragma once
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include <sstream>

namespace sevent
{
    namespace boostserialize
    {
        typedef boost::archive::text_iarchive iarchive_t;
        typedef boost::archive::text_oarchive oarchive_t;

        template<typename T>
        std::string toString(T serializable)
        {
            std::stringstream stream;
            oarchive_t oa(stream);
            oa << serializable;
            return stream.str();
        }

        template<typename T>
        void fromString(T& out, const char* serialized)
        {
            std::stringstream stream;
            stream << serialized;
            iarchive_t ia(stream);
            ia >> out;
        }
    } // namespace boostserialize
} // namespace sevent
