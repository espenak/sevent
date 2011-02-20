#include "EventData.h"

namespace sevent
{
    namespace socket
    {

        EventData::EventData(eventId_t eventid, char *data, dataSize_t dataSize) :
            _eventid(eventid), _data(data), _dataSize(dataSize)
        {
        }

        EventData::~EventData()
        {
        }

        EventData::eventId_t EventData::eventid()
        {
            return _eventid;
        }

        EventData::dataSize_t EventData::dataSize()
        {
            return _dataSize;
        }

        char *EventData::data()
        {
            return _data;
        }

    } // namespace socket
} // namespace sevent
