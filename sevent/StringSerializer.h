#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "socket.h"

namespace sevent
{
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
            static socket::Serialized_ptr serialize(boost::shared_ptr<std::string> str)
            {
                return boost::make_shared< StringSerialized >(str);
            }

            static boost::shared_ptr<std::string> deserialize(char* serialized, uint32_t datasize)
            {
                return boost::make_shared<std::string>(serialized);
            }
    };
} // namespace sevent
