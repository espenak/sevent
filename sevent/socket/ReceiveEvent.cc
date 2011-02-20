#include "ReceiveEvent.h"

namespace sevent
{
    namespace socket
    {

        ReceiveEvent::ReceiveEvent(eventId_t eventid, char *data, dataSize_t dataSize) :
            _eventid(eventid), _data(data), _dataSize(dataSize)
        {
        }

        ReceiveEvent::~ReceiveEvent()
        {
        }

        ReceiveEvent::eventId_t ReceiveEvent::eventid()
        {
            return _eventid;
        }

        ReceiveEvent::dataSize_t ReceiveEvent::dataSize()
        {
            return _dataSize;
        }

        char *ReceiveEvent::data()
        {
            return _data;
        }

    } // namespace socket
} // namespace sevent
