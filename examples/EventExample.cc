#include <iostream>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>
#include <boost/ref.hpp>
#include "sevent/sevent.h"


using namespace sevent;
using sevent::event::Buffer;
using sevent::event::Event;
using sevent::socket::Address;
boost::mutex stream_lock; // Guard the print streams to avoid thread output intertwine
typedef boost::shared_ptr<std::string> String_ptr;


// msgHandler() and dieHandler() handles received events.
// All data is downloaded before the handler is called.
//
// Notice that all handlers get a ReceiveEvent object, which
// contains the eventid. This means that we can use the same handler
// for multiple events!
void msgHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                  event::Event_ptr event)
{
    String_ptr msg = event->first<String_ptr>(serialize::String);
    boost::lock_guard<boost::mutex> lock(stream_lock);

    std::cout << "==================================" << std::endl;
    std::cout << "MSG-event received!" << std::endl;
    std::cout << "Event id:  " << event->eventid() << std::endl;
    std::cout << "Data:      " << *msg << std::endl;
    std::cout << "==================================" << std::endl;
    facade->service()->stop();
}

void dieHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                event::Event_ptr event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "*** DIE-event received ***" << std::endl;
    facade->service()->stop();
}

void msgAckHandler(socket::Facade_ptr facade, socket::Session_ptr session,
                   event::Event_ptr event)
{
    boost::lock_guard<boost::mutex> lock(stream_lock);
    std::cout << "*** MSG-AKC-event received ***" << std::endl;
}


class ConversationChain
{
    public:
        static void onFirst(socket::Facade_ptr facade, socket::Session_ptr session,
                            event::Event_ptr event)
        {
            { // We have to make sure the lock is released before sending the ack, since the same lock is used in the ack handler.
                boost::lock_guard<boost::mutex> lock(stream_lock);
                std::cout << "*** FIRST-event received ***" << std::endl;
            }
            facade->sendEvent(session, Event::make(event::Chain::nextEventId(event)));
        }

        static void onFirstAck(socket::Facade_ptr facade, socket::Session_ptr session,
                               event::Event_ptr event)
        {
            {
                boost::lock_guard<boost::mutex> lock(stream_lock);
                std::cout << "*** FIRST-AKC-event received ***" << std::endl;
            }
            facade->sendEvent(session, Event::make(event::Chain::nextEventId(event)));
        }


        static void onFirstAckAck(socket::Facade_ptr facade, socket::Session_ptr session,
                                  event::Event_ptr event)
        {
            {
                boost::lock_guard<boost::mutex> lock(stream_lock);
                std::cout << "*** FIRST-AKC-ACK-event received ***" << std::endl;
            }
            facade->sendEvent(session, Event::make(event::Chain::nextEventId(event)));
        }

        static void onFirstAckAckAck(socket::Facade_ptr facade, socket::Session_ptr session,
                                  event::Event_ptr event)
        {
            boost::lock_guard<boost::mutex> lock(stream_lock);
            std::cout << "*** FIRST-AKC-ACK-ACK-event received ***" << std::endl;
        }
};

int main(int argc, const char *argv[])
{
    socket::Facade_ptr facade = socket::Facade::make();
    socket::Facade_ptr facade2 = socket::Facade::make();

    // Setup the eventhandlers
    event::HandlerMap_ptr eventHandlerMap = event::HandlerMap::make();
    eventHandlerMap->addEventHandler("sevent::examples::Msg", msgHandler);
    //eventHandlerMap->addEventHandler("sevent::examples::chain.first", ConversationChain::onFirst);
    //eventHandlerMap->addEventHandler("sevent::examples::chain.firstAck", ConversationChain::onFirstAck);
    eventHandlerMap->addEventHandler("sevent::examples::Die", dieHandler);

    event::Chain_ptr chain = event::Chain::make(eventHandlerMap,
                                                "sevent::examples::chain::");
    chain->add(ConversationChain::onFirst);
    chain->add(ConversationChain::onFirstAck);
    chain->add(ConversationChain::onFirstAckAck);
    chain->add(ConversationChain::onFirstAckAckAck);

    // Start 5 worker threads, and use the handler above for incoming events.
    // Worker threads poll for IO-events, and ends up running event-handlers
    // (registered above), so it is a good idea to start new threads or spawn
    // many worker threads if any event-handler does any time-consuming task.
    facade->setWorkerThreads(5, boost::bind(event::simpleAllEventsHandler,
                                            eventHandlerMap,
                                            _1, _2, _3));
    facade2->setWorkerThreads(5, boost::bind(event::simpleAllEventsHandler,
                                             eventHandlerMap,
                                             _1, _2, _3));

    // Create 2 listening sockets. YES we are running both the server and the
    // client in the same process within the same service (a facade contains a
    // single service, and a service is what receives IO-requests).
    // We could of course have created multiple facades, but it would not have
    // made any difference in this example.
    socket::Listener_ptr listener1 = facade->listen(Address::make("127.0.0.1", 9091));
    socket::Listener_ptr listener2 = facade->listen(Address::make("127.0.0.1", 9092));
    socket::Listener_ptr listener3 = facade2->listen(Address::make("127.0.0.1", 3000));

    // Make 2 sessions, one for each listening socket.
    // Notice that two of them is to the same listening socket!
    socket::Session_ptr session1 = facade->connect(Address::make("127.0.0.1", 9091));
    socket::Session_ptr session2 = facade->connect(Address::make("127.0.0.1", 9091));
    socket::Session_ptr session3 = facade->connect(Address::make("127.0.0.1", 9092));
    socket::Session_ptr session4 = facade->connect(Address::make("127.0.0.1", 3000)); // Connect to a listener in facade2

    // All data must be deallocated on destruction, and it must have a
    // serializer. Shared pointers are deallocated on destruction, and we have
    // a StringSerializer (which does nothing to the string).
    String_ptr hello = boost::make_shared<std::string>("Hello");
    String_ptr world = boost::make_shared<std::string>("world");

    // Lets send a couple of events! Note that the received order is not
    // guaranteed, so we might die before all messages are received!
    event::Event_ptr helloEvent = Event::make("sevent::examples::Msg",
                                              Buffer::make(hello, serialize::String));
    event::Event_ptr worldEvent = Event::make("sevent::examples::Msg",
                                              Buffer::make(world, serialize::String));
    facade->sendEvent(session1, helloEvent);
    facade->sendEvent(session4, worldEvent);
    facade->sendEvent(session1, Event::make("sevent::examples::chain::000"));

    // Note that facade->sendEvent is a shortcut for:
    if(facade->isLocalSession(session1)) {
        facade->invokeAllEventsHandler(session1, helloEvent); // Invoke handler directly
    } else {
        session1->sendEvent(helloEvent); // Serialize message, and send it via the session (socket communication)
    }

    // Send the die event, which will invoke the dieHandler and kill the
    // service
    facade->sendEvent(session4, Event::make("sevent::examples::Die"));


    // Always nice to know who you are communicating with..
    {
        boost::lock_guard<boost::mutex> lock(stream_lock);
        std::cout << "#Session 1# Local=" << session1->getLocalEndpointAddress()
                  << " Remote=" << session1->getRemoteEndpointAddress()
                  << "  Is local connection?: " << std::boolalpha << facade->isLocalSession(session1)
                  << std::endl;
        std::cout << "#Session 2# Local=" << session2->getLocalEndpointAddress()
                  << " Remote=" << session2->getRemoteEndpointAddress()
                  << "  Is local connection?: " << std::boolalpha << facade->isLocalSession(session2)
                  << std::endl;
        std::cout << "#Session 3# Local=" << session3->getLocalEndpointAddress()
                  << " Remote=" << session3->getRemoteEndpointAddress()
                  << "  Is local connection?: " << std::boolalpha << facade->isLocalSession(session3)
                  << std::endl;
        std::cout << "#Session 4# Local=" << session4->getLocalEndpointAddress()
                  << " Remote=" << session4->getRemoteEndpointAddress()
                  << "  Is local connection?: " << std::boolalpha << facade->isLocalSession(session4)
                  << std::endl;
    }

    // Wait for all work to finish. In this example this will happen
    // when the dieHandler calls facade->service()->stop().
    facade->joinAllWorkerThreads();

    return 0;
}
