#pragma once
#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <arpa/inet.h>

namespace sevent
{
    namespace event
    {
        class NumericEventId;
        typedef boost::shared_ptr<NumericEventId> NumericEventId_ptr;
        class NumericEventId
        {
            public:
                typedef uint32_t value_type;
                typedef uint32_t value_typeref;
                typedef uint32_t header_type;
                typedef uint32_t header_network_type;
            public:
                /**
                 * @param body The serialized body. Goes out of context.
                 */
                static NumericEventId_ptr makeFromNetwork(header_type header, char* body)
                {
                    return boost::make_shared<NumericEventId>(header);
                }

                /** Number of bytes occupied by the serialized header. */
                static unsigned headerSerializedSize()
                {
                    return sizeof(uint32_t);
                }

                static header_type deserializeHeader(header_network_type header)
                {
                    return ntohl(header);
                }

                /** Number of bytes occupied by the serialized body. */
                static uint8_t bodySerializedSize(header_type header)
                {
                    return 0;
                }

                static bool hasBody()
                {
                    return false;
                }

            public:
                NumericEventId(value_type value) :
                    _value(value) {}

                header_network_type serializeHeader()
                {
                    return htonl(_value);
                }

                const value_type& value()
                {
                    return _value;
                }

                
            private:
                value_type _value;
        };

        typedef NumericEventId eventid_t;
        typedef boost::shared_ptr<eventid_t> eventid_t_ptr;
    } // namespace socket
} // namespace sevent
