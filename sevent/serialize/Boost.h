#pragma once

#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include "Result.h"
#include "Pair.h"

namespace sevent
{
    namespace serialize
    {
        typedef boost::archive::text_iarchive iarchive_t;
        typedef boost::archive::text_oarchive oarchive_t;

        template<typename T>
        struct BoostSerialize
        {
            typedef T value_type;
            typedef boost::shared_ptr<value_type> value_ptr;

            static serialize::BaseResult_ptr serialize(boost::any& data)
            {
                value_ptr dataCast = boost::any_cast<value_ptr>(data);
                std::stringstream stream;
                oarchive_t oa(stream);
                oa << *dataCast;
                return boost::make_shared<serialize::StringResult>(stream.str());
            }

            static boost::any deserialize(datastruct::MutableCharArray_ptr serialized)
            {
                value_ptr out = boost::make_shared<value_type>();
                std::stringstream stream;
                stream << serialized->data;
                iarchive_t ia(stream);
                ia >> *out;
                return out;
            }
        };

        template<typename T>
        struct Boost: Pair
        {
            typedef T value_type;
            Boost() : Pair(BoostSerialize<value_type>::serialize,
                               BoostSerialize<value_type>::deserialize)
            {}
        };
    } // namespace serialize
} // namespace sevent
