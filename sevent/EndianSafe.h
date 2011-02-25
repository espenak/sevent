/** /namespace sevent::endiansafe
 *
 * A endianess framework where we assume one will send data in pairs of
 * (endianess,data), and decode it if needed on the receiving side.
 *
 * This appoach saves a lot of resoures compared to frameworks where data
 * is encoded in one endian, and always encoded it, even if both sides
 * use the same endianess.
 */
#pragma once
#include "socket/ConstBuffer.h"
#include "socket/ReceiveEvent.h"
#include <boost/detail/endian.hpp>

namespace sevent
{
    namespace endiansafe
    {
        enum Endianess {
            SEVENT_LITTLE_ENDIAN = 1, // Need to prefix because [LITTLE|BIG]_ENDIAN is a non-namespaced global on some systems.
            SEVENT_BIG_ENDIAN = 2,
            SEVENT_MIDDLE_ENDIAN = 3
        };


#ifdef BOOST_LITTLE_ENDIAN
            static uint8_t my_endianess = SEVENT_LITTLE_ENDIAN;
#elif BOOST_BIG_ENDIAN
            static uint8_t my_endianess = SEVENT_BIG_ENDIAN;
#elif BOOST_PDP_ENDIAN
            static uint8_t my_endianess = SEVENT_MIDDLE_ENDIAN;
#else
#error "Unsupported endianess detected."
#endif

        /** Get the current endianess. */
        inline Endianess myEndianess()
        {
            return (Endianess)my_endianess;
        }


        void addToConstBufferVector(socket::ConstBufferVector& vec,
                               socket::ConstBuffer b)
        {
            vec.push_back(socket::ConstBuffer((char*)&my_endianess, 1));
            vec.push_back(b);
        }


        static void endianCheck(Endianess endianess)
        {
            if(!myEndianess() == endianess)
            {
                throw new std::runtime_error("We currently do not support endian-decoding.");
            }
        }

        /** Decode a buffer with the given endianess. */
        template<typename T>
        socket::MutableBuffer_ptr decode(socket::MutableBuffer_ptr buf,
                                         Endianess endianess)
        {
            endianCheck(endianess);
            return buf;
        }

        /** Shortcut for popping the data+endianess from the end of a buffer,
         * and decode() the data.
         * This is the reccommended opposite to addToConstBufferVector().*/
        template<typename T>
        socket::MutableBuffer_ptr popBackAndDecode(socket::ReceiveEvent& event)
        {
            socket::MutableBuffer_ptr buf = event.popBack();
            Endianess endianess = *(event.popBack()->data<Endianess*>());
            return decode<T>(buf, endianess);
        }


    } // namespace endiansafe
} // namespace sevent
