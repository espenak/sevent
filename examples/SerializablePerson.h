#pragma once
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>


/** A serializable person! */
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
