#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

#include "sevent/socket.h"


using namespace sevent;
enum EventIds
{
    HELLO_ID = 10,
    DIE_ID = 20
};



int main(int argc, const char *argv[])
{
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();

    // Make 2 sessions, one for each listening socket.
    // Notice that two of them is to the same listening socket!
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));

    // Lets send a couple of events! Note that the received order is not
    // guaranteed.
    session->sendEvent(HELLO_ID, socket::ConstBuffer("Hello", 6));
    session->sendEvent(HELLO_ID, socket::ConstBuffer("Cruel", 6));
    session->sendEvent(HELLO_ID, socket::ConstBuffer("World", 6));
    session->sendEvent(DIE_ID, socket::ConstBuffer(0, 0));

    facade->joinAllWorkerThreads();
    //boost::this_thread::sleep(boost::posix_time::seconds(2));

    return 0;
}
