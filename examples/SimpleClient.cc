#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include "sevent/sevent.h"
#include "sevent/serialize/String.h"
#include "sevent/serialize/Boost.h"
#include "SerializablePerson.h"


using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;


int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: " << argv[0]
            << " <server-ip> <server-port>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    Facade_ptr facade = Facade::make();
    Session_ptr session = facade->connect(Address::make(host, port));

    // Event data
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr hello = boost::make_shared<std::string>("Hello");
    Person_ptr superman = boost::make_shared<Person>("Superman", 39);

    // The events
    Event_ptr helloEvent = Event::make("example::Msg",
               Buffer::make(hello, serialize::String));
    Event_ptr supermanEvent = Event::make("example::Person",
               Buffer::make(superman, serialize::Boost<Person>()));

    // Send the events
    facade->sendEvent(session, helloEvent);
    facade->sendEvent(session, supermanEvent);
    facade->sendEvent(session, Event::make("example::Die"));
    return 0;
}
