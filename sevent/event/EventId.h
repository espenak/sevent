#pragma once
#include <boost/shared_ptr.hpp>
#include "NumericEventId.h"
#include "StringEventId.h"

namespace sevent
{
    namespace event
    {
#ifdef SEVENT_USE_INT_ID
        typedef NumericEventId eventid_t;
#else
        typedef StringEventId eventid_t;
#endif
        typedef boost::shared_ptr<eventid_t> eventid_t_ptr;
    } // namespace socket
} // namespace sevent
