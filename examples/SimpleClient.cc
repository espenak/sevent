#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"
#include "SerializablePerson.h"

using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;
using namespace sevent::datastruct;

int main(int argc, const char *argv[]) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0]
            << " <server-addr>" << std::endl;
        return 1;
    }
    std::string serverAddr(argv[1]);

    Facade_ptr facade = Facade::make();
    Session_ptr session = facade->connect(Address::make(serverAddr));

    // Event data
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr hello = boost::make_shared<std::string>(
                                        "Hello");
    Person_ptr superman = boost::make_shared<Person>(
                                        "Superman", 39);
    boost::shared_array<uint32_t> array = boost::shared_array<uint32_t>(new uint32_t[3]);
    array[0] = 10;
    array[1] = 20;
    array[2] = 30;
    Uint32SharedArray_ptr arrayContainer = boost::make_shared<Uint32SharedArray>(array, 3);

    // The events
    Event_ptr helloEvent = Event::make("example::Msg",
               Buffer::make(hello,
                            serialize::String));
    Event_ptr supermanEvent = Event::make("example::Person",
               Buffer::make(superman,
                            serialize::Boost<Person>()));
    Event_ptr arrayEvent = Event::make("example::Array",
               Buffer::make(arrayContainer,
                            serialize::Uint32SharedArray));

    // Send the events
    facade->sendEvent(session, helloEvent);
    facade->sendEvent(session, supermanEvent);
    facade->sendEvent(session, arrayEvent);
    facade->sendEvent(Address::make(serverAddr), helloEvent);
    facade->sendEvent(session, Event::make("example::Die"));
    return 0;
}
