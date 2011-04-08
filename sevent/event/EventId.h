#pragma once
#include <stdint.h>
#include <boost/shared_ptr.hpp>

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
                static NumericEventId_ptr make(value_typeref value)
                {
                    return boost::make_shared<NumericEventId>(value);
                }

                //static makeFromNetwork()
                //{
                    
                //}

                //static header_type deserializeHeader(const char* header)
                //{
                    //header_type header = static_cast<header_type>(*header);
                //}

                //static uint8_t bodySize( )
                //{
                    //return 0;
                //}
                static unsigned headerNetworkSize()
                {
                    return sizeof(uint32_t);
                }

            public:
                NumericEventId(value_type value) :
                    _value(value) {}

                // TODO: use a pair?
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
