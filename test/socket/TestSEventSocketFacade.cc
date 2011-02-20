#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestSevent
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE( TestSEventMain )
{
    BOOST_REQUIRE_EQUAL(1, 2);
}
