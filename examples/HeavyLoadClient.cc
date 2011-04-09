#include <boost/lexical_cast.hpp>
#include <boost/timer.hpp>
#include "sevent/sevent.h"
#include "Instrumentation.h"

using namespace sevent;
using namespace sevent::socket;
using namespace sevent::event;
typedef boost::shared_ptr<std::string> String_ptr;


unsigned getSerializedSize()
{
    Instrumentation_ptr data = boost::make_shared<Instrumentation>(0, 0.40f);
    serialize::Boost<Instrumentation> serializer;
    boost::any anyData = data;
    serialize::BaseResult_ptr serialized = serializer.serializeFunc(anyData);
    return serialized->size();
}

int main(int argc, const char *argv[])
{
    if(argc < 4)
    {
        std::cout << "Usage: " << argv[0]
            << " <server-ip> <server-port> <numevents>" << std::endl;
        return 1;
    }
    std::string host(argv[1]);
    unsigned short port = boost::lexical_cast<unsigned short>(argv[2]);
    unsigned numEvents = boost::lexical_cast<unsigned>(argv[3]);

    Facade_ptr facade = Facade::make();
    Session_ptr session = facade->connect(Address::make(host, port));

    // Event data
    String_ptr msg = boost::make_shared<std::string>(1048576, 'x');

    //unsigned serializedSize = getSerializedSize();
    unsigned serializedSize = msg->size();
    double totalSize = (serializedSize + 14) * numEvents;
    double totalSizeMB = (totalSize / 1024.0f / 1024.0f);
    std::cout << "Transferring " << numEvents << " events "
        << "(" << totalSizeMB << "MB)"<< std::endl;

    // Send the events
    boost::timer timer;
    for(unsigned id = 0; id < numEvents; id++)
    {
        //Instrumentation_ptr instrdata = boost::make_shared<Instrumentation>(id, 0.40f);
        //Buffer_ptr buffer = Buffer::make(instrdata,
                                         //serialize::Boost<Instrumentation>());
        //Event_ptr instrdataEvent = Event::make("heavy::instr",
                                               //buffer);
        //facade->sendEvent(session, instrdataEvent);
        
        //facade->sendEvent(session, Event::make("heavy::empty"));
        session->sendEvent(Event::make("heavy::string",
                                       Buffer::make(msg, serialize::String)));
    }

    // Calculate Kb/s
    double sec = timer.elapsed();
    double elapsedMbPerSec = totalSizeMB/sec;
    std::cout << sec << std::endl;
    std::cout << elapsedMbPerSec << "MB/s" << std::endl;
    return 0;
}
