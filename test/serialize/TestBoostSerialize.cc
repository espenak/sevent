#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBoostSerialize
#include <boost/test/unit_test.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/any.hpp>
#include <cstring>
#include <iostream>
#include "sevent/serialize/Boost.h"

using namespace sevent;
using namespace sevent::datastruct;

class Person
{
    public:
        unsigned short age;
        std::string name;
    public:
        Person() {}
        Person(const std::string& name_, unsigned short age_) :
            name(name_), age(age_)
        {}
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & age & name;
        }
};
typedef boost::shared_ptr<Person> Person_ptr;


BOOST_AUTO_TEST_CASE(TestBoostSerializeSanity)
{
    serialize::Boost<Person> PersonSerializer;

    Person_ptr supermanIn = boost::make_shared<Person>("Superman", 30);
    boost::any supermanInAny = supermanIn;
    serialize::BaseResult_ptr serialized = PersonSerializer.serializeFunc(supermanInAny);

    MutableCharArray_ptr ma = boost::make_shared<MutableCharArray>(const_cast<char*>(serialized->data()),
                                                                   serialized->size());

    Person_ptr supermanOut = boost::any_cast<Person_ptr>(PersonSerializer.deserializeFunc(ma));
    BOOST_REQUIRE_EQUAL(supermanOut->name, "Superman");
    BOOST_REQUIRE_EQUAL(supermanOut->age, 30);
}
