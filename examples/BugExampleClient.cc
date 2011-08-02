#include <boost/lexical_cast.hpp>
#include "sevent/sevent.h"

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


    Session_ptr session;
    for(int i = 0; i < 100000; i++)
    {
        session = facade->connect(Address::make(serverAddr));
        // Event data
        typedef boost::shared_ptr<std::string> String_ptr;
        String_ptr hello = boost::make_shared<std::string>(100000000, 'x');
        Event_ptr helloEvent = Event::make("example::Msg",
                                           Buffer::make(hello,
                                                        serialize::String));
        facade->sendEvent(session, helloEvent);
    }
    return 0;
}
