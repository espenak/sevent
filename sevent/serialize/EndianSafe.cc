#include "EndianSafe.h"
#include <boost/detail/endian.hpp>

namespace sevent
{
    namespace serialize
    {

        /** Get the current endianess. */
        Endianess myEndianess()
        {
#ifdef BOOST_LITTLE_ENDIAN
            return SEVENT_LITTLE_ENDIAN;
#elif defined BOOST_BIG_ENDIAN
            return SEVENT_BIG_ENDIAN;
//#elif defined BOOST_PDP_ENDIAN
            //return SEVENT_MIDDLE_ENDIAN;
#else
#error "Unsupported endianess detected."
#endif
        }

    }
} // namespace sevent
