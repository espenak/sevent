#pragma once
#include <stdint.h>

namespace sevent
{
    namespace serialize
    {
        enum Endianess {
            SEVENT_LITTLE_ENDIAN = 1, // Need to prefix because [LITTLE|BIG]_ENDIAN is a non-namespaced global on some systems.
            SEVENT_BIG_ENDIAN = 2,
            SEVENT_MIDDLE_ENDIAN = 3
        };

        /** Get the current endianess. */
        Endianess myEndianess();

        //template<typename T> T endianSwap(T num);
        template<typename T> T endianSwap(T num)
        {
            // http://stackoverflow.com/questions/1826159/swapping-two-variable-value-without-using-3rd-variable
            T result;
            uint8_t* in = reinterpret_cast<uint8_t*>(&num);
            uint8_t* out = reinterpret_cast<uint8_t*>(&result);
            uint8_t size = sizeof(T);
            for(int i = 0; i < size; i++)
            {
                out[i] = in[size-i-1];
            }
            return result;
        }
    }
} // namespace sevent
