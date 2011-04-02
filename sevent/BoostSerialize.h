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
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "sevent/socket/ConstBuffer.h"
#include "sevent/socket/Buffer.h"

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

        template<typename T>
        class Serialized : public socket::Serialized
        {
            private:
                std::string result;
            public:
                Serialized(const T& serializable)
                {
                    std::stringstream stream;
                    oarchive_t oa(stream);
                    oa << serializable;
                    result = stream.str();
                }

                const char* data() const
                {
                    return result.c_str();
                }

                uint32_t size() const
                {
                    return result.size();
                }
        };

        class Serializer
        {
            public:
                template<typename T>
                static socket::Serialized_ptr serialize(boost::shared_ptr<T> data)
                {
                    return boost::make_shared< Serialized<T> >(*data);
                }

                template<typename T>
                static socket::Buffer<T, Serializer> deserialize(const char* serialized, uint32_t datasize)
                {
                    boost::shared_ptr<T> out = boost::make_shared<T>();
                    std::stringstream stream;
                    stream << serialized;
                    iarchive_t ia(stream);
                    ia >> *out;
                    return socket::Buffer<T, Serializer>(out);
                }
        };
    } // namespace boostserialize
} // namespace sevent
