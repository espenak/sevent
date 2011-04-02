#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "socket.h"

namespace sevent
{
    template<typename T>
    class StringSerialized : public socket::Serialized
    {
        private:
            boost::shared_ptr<std::string> _str;
        public:
            StringSerialized(boost::shared_ptr<std::string> str) : _str(str)
        {}

            const char* data() const
            {
                return _str->c_str();
            }

            uint32_t size() const
            {
                return _str->size();
            }
    };

    class StringSerializer
    {
        public:
            template<typename T>
            static socket::Serialized_ptr serialize(boost::shared_ptr<T> str)
            {
                return boost::make_shared< StringSerialized<T> >(str);
            }

            template<typename T>
            static socket::Buffer<T, StringSerializer> deserialize(const char* serialized, uint32_t datasize)
            {
                return socket::Buffer<T, StringSerializer>(boost::make_shared<std::string>(serialized));
            }
    };
} // namespace sevent
