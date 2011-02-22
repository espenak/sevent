#include "SendEvent.h"

namespace sevent
{
    namespace socket
    {
        AbstractSendEvent::AbstractSendEvent(eventId_t eventid) :
            _eventid(eventid)
        {}

        AbstractSendEvent::eventId_t AbstractSendEvent::eventid() const
        {
            return _eventid;
        }

        //SendEventVector::SendEventVector(eventId_t eventid, ConstBufferVector& buffers) :
            //_eventid(eventid), _dataBuffers(buffers)
        //{
        
        //}

        //const ConstBufferVector_ptr SendEventVector::buffers() const
        //{
            //return _dataBuffers;
        //}




        SendEvent::SendEvent(eventId_t eventid, const char *data, bufsize_t dataSize) :
            AbstractSendEvent(eventid), _const_buf(ConstBuffer(data, dataSize))
        {}

        bufsize_t SendEvent::size() const
        {
            return 1;
        }

        const ConstBuffer& SendEvent::at(unsigned index) const
        {
            return _const_buf;
        }

    } // namespace socket
} // namespace sevent
