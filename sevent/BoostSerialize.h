#pragma once

/** \namespace sevent::boostserialize
 *
 * Simplifies serialization with boost::serialize.
 *
 * \section Example
 *
 * \include TestBoostSerialize.cc
 */

#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include <sstream>
#include "sevent/socket/ConstBuffer.h"

namespace sevent
{
    namespace boostserialize
    {
        typedef boost::archive::text_iarchive iarchive_t;
        typedef boost::archive::text_oarchive oarchive_t;

        /** Serialize a serializable object using boost::archive::text_oarchive. */
        template<typename T>
        std::string toString(const T& serializable)
        {
            std::stringstream stream;
            oarchive_t oa(stream);
            oa << serializable;
            return stream.str();
        }

        /** Serialize a serializable object and put the result in a
         * sevent::socket::ConstBuffer. */
        template<typename T>
        socket::ConstBuffer toConstBuffer(const T& serializable)
        {
            std::string result = toString(serializable);
            return socket::ConstBuffer(result.c_str(), result.size()+1);
        }

        /** Deserialize an object serialized with toString() */
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
