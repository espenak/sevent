#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include "sevent/sevent.h"
#include "sevent/serialize/String.h"
#include "sevent/serialize/Boost.h"
#include "SerializablePerson.h"


using namespace sevent;
using sevent::event::Event;
using sevent::event::Buffer;


enum EventIds
{
    HELLO_ID = 10,
    PERSON_ID = 15,
    DIE_ID = 20
};


int main(int argc, const char *argv[])
{
    if(argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <server-ip> <server-port>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed. Note that we can send more than one buffer in a single
    // event, see examples/showcase/ for examples.

    // Event data
    typedef boost::shared_ptr<std::string> String_ptr;
    String_ptr hello = boost::make_shared<std::string>("Hello");
    String_ptr cruel = boost::make_shared<std::string>("supercruel");
    String_ptr world = boost::make_shared<std::string>("world!");
    Person_ptr superman = boost::make_shared<Person>("Superman", 39);

    // Send the events
    session->sendEvent(Event::make(HELLO_ID,
                                   Buffer::make(hello, serialize::String)));
    session->sendEvent(Event::make(HELLO_ID,
                                   Buffer::make(cruel, serialize::String)));
    session->sendEvent(Event::make(HELLO_ID,
                                   Buffer::make(world, serialize::String)));
    session->sendEvent(Event::make(PERSON_ID,
                                   Buffer::make(superman, serialize::Boost<Person>())));
    session->sendEvent(Event::make(DIE_ID));
    return 0;
}
