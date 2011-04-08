#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestAsciiPack
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <stdint.h>

typedef uint32_t block_type;
unsigned size = 32;
unsigned bits_per_char = 5;
unsigned per_block = size/bits_per_char;
uint32_t mask = 0x1f;

uint32_t pack(const char* values)
{
    uint32_t result = 0;
    for(int i = 0; i < per_block; i++)
    {
        uint8_t numvalue = values[i] - 'a';
        result |= numvalue;
        if(i!= (per_block-1))
            result = result << bits_per_char;
    }
    return result;
}

void unpack(uint32_t packed, char* values)
{
    for(int i = 0; i < per_block; i++)
    {
        uint32_t c = packed >> (i*bits_per_char);
        c &= mask;
        values[per_block - 1 - i] = c + 'a';
    }
}


BOOST_AUTO_TEST_CASE(TestAsciiPack)
{
    //BOOST_REQUIRE_EQUAL(, );
    char out[7];
    out[6] = '\0';
    unpack(pack("hellow"), out);
    std::cout << out << std::endl;
}
