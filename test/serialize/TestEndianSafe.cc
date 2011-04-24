#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEndianSafe
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include <cstring>
#include <iostream>
#include "sevent/serialize/EndianSafe.h"

using namespace sevent::serialize;


BOOST_AUTO_TEST_CASE(TestEndianSafeSanity)
{
    // 0x 00 00 00 02
    // 0x 02 00 00 00
    BOOST_REQUIRE_EQUAL(endianSwap<uint32_t>(0x2), 0x2000000);

    BOOST_REQUIRE_EQUAL(endianSwap<uint16_t>(0x2), 0x200);
}
