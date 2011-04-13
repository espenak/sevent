#include "EndianSafe.h"
#include <boost/detail/endian.hpp>

namespace sevent
{
    namespace serialize
    {

        /** Get the current endianess. */
        inline Endianess myEndianess()
        {
#ifdef BOOST_LITTLE_ENDIAN
            return SEVENT_LITTLE_ENDIAN;
#elif BOOST_BIG_ENDIAN
            return SEVENT_BIG_ENDIAN;
#else
#error "Unsupported endianess detected."
#endif
        }

    }
} // namespace sevent
