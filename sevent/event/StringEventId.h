#pragma once
#include <stdint.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include "EventIdBodySerialized.h"

namespace sevent
{
    namespace event
    {
        class StringEventId;
        typedef boost::shared_ptr<StringEventId> StringEventId_ptr;
        class StringEventId
        {
            public:
                typedef std::string value_type;
                typedef const value_type& value_typeref;
                typedef uint8_t header_type;
                typedef uint8_t header_network_type;
            public:
                /**
                 * @param body The serialized body. Goes out of context.
                 */
                static StringEventId_ptr makeFromNetwork(header_type header, char* body)
                {
                    return boost::make_shared<StringEventId>(std::string(body));
                }

                /** Number of bytes occupied by the serialized header. */
                static unsigned headerSerializedSize()
                {
                    return 1;
                }

                static header_type deserializeHeader(header_network_type header)
                {
                    return header;
                }

                /** Number of bytes occupied by the serialized body. */
                static uint8_t bodySerializedSize(header_type header)
                {
                    return header;
                }

                static bool hasBody()
                {
                    return true;
                }

            public:
                StringEventId(value_typeref value) :
                    _value(value) {}

                header_network_type serializeHeader()
                {
                    return static_cast<uint8_t>(_value.size()+1);
                }

                EventIdBodySerialized serializeBody()
                {
                    return EventIdBodySerialized(_value.size()+1, _value.c_str());
                }

                const value_type& value()
                {
                    return _value;
                }

                
            private:
                value_type _value;
        };
    } // namespace event
} // namespace sevent
