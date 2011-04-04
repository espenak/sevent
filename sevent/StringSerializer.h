#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <cstring>
#include "socket.h"

namespace sevent
{
    class StringSerialized : public socket::Serialized
    {
        private:
            boost::shared_ptr<char> _str;
        public:
            StringSerialized(boost::shared_ptr<char> str) : _str(str) {}

            const char* data() const
            {
                return _str.get();
            }

            uint32_t size() const
            {
                return std::strlen(_str.get());
            }
    };

    class StringSerializer
    {
        public:
            static socket::Serialized_ptr serialize(boost::shared_ptr<char> str)
            {
                return boost::make_shared<StringSerialized>(str);
            }

            static boost::shared_ptr<char> deserialize(char* serialized, uint32_t datasize)
            {
                boost::shared_ptr<char> str = boost::shared_ptr<char>(serialized);
                return str;
            }
    };
} // namespace sevent
