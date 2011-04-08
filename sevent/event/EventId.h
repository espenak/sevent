#pragma once
#include <stdint.h>

namespace sevent
{
    namespace event
    {
        class NumericEventId
        {
            public:
                typedef uint32_t value_type;
                typedef uint32_t value_typeref;
                typedef uint32_t header_network_t;
            public:
                NumericEventId(value_type value) :
                    _value(value) {}

                header_network_t headerNetworkSafe()
                {
                    return htonl(_value);
                }

                unsigned headerNetworkSize()
                {
                    return sizeof(uint32_t);
                }

                bool hasBody()
                {
                    return false;
                }

                uint8_t bodySize()
                {
                    return 0;
                }

                const value_type& value()
                {
                    return _value;
                }

                
            private:
                value_type _value;
        };

        typedef NumericEventId eventid_t;
    } // namespace socket
} // namespace sevent
