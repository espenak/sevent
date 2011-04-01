#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBoostSerialize
#include <boost/test/unit_test.hpp>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <iostream>
#include "sevent/BoostSerialize.h"

using namespace sevent;
using sevent::boostserialize::Serializer;


/** Serializable Person */
class Person
{
    public:
        Person() {}
        Person(const std::string& name, unsigned short age) :
            _name(name), _age(age)
        {}
        unsigned short age() const { return _age; }
        const std::string& name() const { return _name; }

    private:
        unsigned short _age;
        std::string _name;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & _age & _name;
        }
};
typedef boost::shared_ptr<Person> Person_ptr;


BOOST_AUTO_TEST_CASE( TestBoostSerializeCore )
{
    // Serialize
    Person superman("Superman", 40);
    std::string serialized = sevent::boostserialize::toString(superman);

    // Deserialize
    Person supermanDeserialized;
    sevent::boostserialize::fromString(supermanDeserialized, serialized.c_str());

    BOOST_REQUIRE_EQUAL(supermanDeserialized.name(), "Superman");
    BOOST_REQUIRE_EQUAL(supermanDeserialized.age(), 40);
}


BOOST_AUTO_TEST_CASE( TestBoostSerializeConstBuffer )
{
    // Serialize
    sevent::socket::ConstBuffer b = sevent::boostserialize::toConstBuffer(Person("Superman", 40));

    // Deserialize
    Person supermanDeserialized;
    sevent::boostserialize::fromString(supermanDeserialized,
                                       static_cast<const char*>(b.data()));

    BOOST_REQUIRE_EQUAL(supermanDeserialized.name(), "Superman");
    BOOST_REQUIRE_EQUAL(supermanDeserialized.age(), 40);
}

BOOST_AUTO_TEST_CASE( TestBoostSerializer )
{
    // Serialize
    boost::shared_ptr<Person> superman = boost::make_shared<Person>("Superman", 40);
    socket::Serialized_ptr serialized = boostserialize::Serializer::serialize<Person>(superman);

    // Deserialize
    Person_ptr supermanDeserialized = Serializer::deserialize<Person>(serialized->data(),
                                                                      serialized->size()).data();
    BOOST_REQUIRE_EQUAL(supermanDeserialized->name(), "Superman");
    BOOST_REQUIRE_EQUAL(supermanDeserialized->age(), 40);
}
