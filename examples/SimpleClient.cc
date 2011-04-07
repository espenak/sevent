#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include "sevent/sevent.h"
#include "sevent/serialize/String.h"
#include "SerializablePerson.h"


using namespace sevent;
enum EventIds
{
    HELLO_ID = 10,
    PERSON_ID = 15,
    DIE_ID = 20
};


int main(int argc, const char *argv[])
{
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed. Note that we can send more than one buffer in a single
    // event, see examples/showcase/ for examples.
    session->sendEvent(event::Event::make(HELLO_ID,
                                          event::Buffer::make(boost::make_shared<std::string>("Hello"),
                                                              serialize::String)));
    //session->sendEvent(HELLO_ID, socket::ConstBuffer("Hello", 6));
    //session->sendEvent(HELLO_ID, socket::ConstBuffer("Cruel", 6));
    ////session->sendEvent(PERSON_ID, boostserialize::toConstBuffer(Person("Superman", 39)));
    //session->sendEvent(HELLO_ID, socket::ConstBuffer("World", 6));
    //session->sendEvent(DIE_ID, socket::ConstBuffer(0, 0));
    return 0;
}
