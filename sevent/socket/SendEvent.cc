#include "SendEvent.h"

namespace sevent
{
    namespace socket
    {

        SendEvent::SendEvent(eventId_t eventid, const char *data, bufsize_t dataSize) :
            _eventid(eventid), _data(data), _dataSize(dataSize)
        {
        }

        SendEvent::~SendEvent()
        {
        }

        SendEvent::eventId_t SendEvent::eventid() const
        {
            return _eventid;
        }

        SendEvent::bufsize_t SendEvent::dataSize() const
        {
            return _dataSize;
        }

        const char* SendEvent::data() const
        {
            return _data;
        }

    } // namespace socket
} // namespace sevent
