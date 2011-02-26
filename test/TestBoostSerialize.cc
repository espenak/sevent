#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestBoostSerialize
#include <boost/test/unit_test.hpp>
#include <string>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <iostream>
#include "sevent/BoostSerialize.h"


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


BOOST_AUTO_TEST_CASE( TestBoostSerializeMain )
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
