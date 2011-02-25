#pragma once
#include <boost/serialization/string.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


class Person
{
    public:
        Person() {}
        ~Person() {}
        Person(int id, const std::string& name, const std::string& email)
            : _id(id), _name(name), _email(email) {}

        int getId() const
        {
            return _id;
        }
        const std::string& getName() const
        {
            return _name;
        }
        const std::string& getEmail() const
        {
            return _email;
        }

    private:
        std::string _name;
        int _id;
        std::string _email;

    private:
        // The boost::serialization part of the code
        friend class boost::serialization::access;
        template<class Archive> void serialize(Archive& ar, const unsigned int version)
        {
            ar & _id & _name & _email;
        }
};

std::ostream& operator<< (std::ostream& out, const Person& person)
{
    out << person.getId() << ") " <<  person.getName()
        << "<" << person.getEmail() << ">";
    return out;
}
