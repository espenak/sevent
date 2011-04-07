#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestStringSerialize
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include <cstring>
#include <iostream>
#include "sevent/serialize/String.h"

using namespace sevent;


boost::shared_ptr<std::string> createString()
{
    return boost::make_shared<std::string>("Hello world");
}


BOOST_AUTO_TEST_CASE(TestStringSerializeSanity)
{
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr sIn = createString();
    BOOST_REQUIRE_EQUAL(*sIn, "Hello world");

    boost::any sInAny = sIn;
    serialize::BaseResult_ptr serialized = serialize::String.serializeFunc(sInAny);
    BOOST_REQUIRE_EQUAL(serialized->data(), "Hello world");

    datastruct::MutableCharArray_ptr ma = boost::make_shared<datastruct::MutableCharArray>(const_cast<char*>(serialized->data()),
                                                                                          serialized->size());
    String_ptr sOut = boost::any_cast<String_ptr>(serialize::String.deserializeFunc(ma));
    BOOST_REQUIRE_EQUAL(*sOut, "Hello world");
}
