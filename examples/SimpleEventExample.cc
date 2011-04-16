#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include <string>
#include "sevent/sevent.h"
#include "SerializablePerson.h"

using namespace sevent;
using namespace sevent::event;
typedef boost::shared_ptr<std::string> String_ptr;

int main(int argc, const char *argv[]) {
    String_ptr helloIn, helloOut;
    Person_ptr supermanIn, supermanOut;

    helloIn = boost::make_shared<std::string>("Hello");
    supermanIn = boost::make_shared<Person>("Superman", 39);
    
    Event_ptr event = Event::make("example::MyEvent");
    event->push_back(Buffer::make(helloIn,
                                  serialize::String));
    event->push_back(Buffer::make(supermanIn,
                                 serialize::Boost<Person>()));

    helloOut = event->first<String_ptr>(serialize::String);
    supermanOut = event->at<Person_ptr>(1,
                                serialize::Boost<Person>());
    std::cout << *helloOut << std::endl;
    std::cout << supermanOut->name << std::endl;
    return 0;
}
