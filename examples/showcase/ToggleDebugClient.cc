#include <iostream>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"
#include "Shared.h"


using namespace sevent;

int main(int argc, const char *argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <host IP> <host port>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);

    socket::Facade_ptr facade = socket::Facade::make();
    socket::Session_ptr session = facade->connect(socket::Address::make(host, port));
    session->sendEvent(TOGGLE_DEBUG_EVENT_ID, socket::ConstBuffer(0,0));

    return 0;
}
