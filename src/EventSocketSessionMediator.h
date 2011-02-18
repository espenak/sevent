#pragma once

#include "EventData.h"
#include "SocketSession.h"
#include <boost/shared_ptr.hpp>

namespace sevent
{
namespace socket
{

class EventSocketSessionMediator;
typedef boost::shared_ptr<EventSocketSessionMediator>
        EventSocketSessionMediator_ptr;

/** Somewhat decouples eventhandlers from SocketSession by only giving events
 * access to the functions they need.
 */
class EventSocketSessionMediator
{
    public:
        static EventSocketSessionMediator_ptr make(
                SocketSession_ptr socketSession);
    public:
        EventSocketSessionMediator(SocketSession_ptr socketSession);
        virtual ~EventSocketSessionMediator();
        void sendEvent(EventData eventData);
    private:
        SocketSession_ptr _socketSession;
};

}
