#pragma once

namespace sevent
{
    namespace event
    {
        struct EventIdBodySerialized
        {
            unsigned size;
            const char* data;
            EventIdBodySerialized(unsigned size_, const char* data_) :
                size(size_), data(data_) {}
        };

    } // namespace event
} // namespace sevent
