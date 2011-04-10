#include "SharedArray.h"

namespace sevent
{
    namespace serialize
    {
        Pair CharSharedArray(SharedArraySerializer<char>::serialize,
                             SharedArraySerializer<char>::deserialize);

        Pair Int8SharedArray(SharedArraySerializer<int8_t>::serialize,
                               SharedArraySerializer<int8_t>::deserialize);
        Pair Uint8SharedArray(SharedArraySerializer<uint8_t>::serialize,
                               SharedArraySerializer<uint8_t>::deserialize);

        Pair Int16SharedArray(SharedArraySerializer<int16_t>::serialize,
                               SharedArraySerializer<int16_t>::deserialize);
        Pair Uint16SharedArray(SharedArraySerializer<uint16_t>::serialize,
                               SharedArraySerializer<uint16_t>::deserialize);

        Pair Int32SharedArray(SharedArraySerializer<int32_t>::serialize,
                               SharedArraySerializer<int32_t>::deserialize);
        Pair Uint32SharedArray(SharedArraySerializer<uint32_t>::serialize,
                               SharedArraySerializer<uint32_t>::deserialize);
    } // namespace serialize
} // namespace sevent
