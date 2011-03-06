#include "ReceiveEvent.h"

namespace sevent
{
    namespace socket
    {

        ReceiveEvent::ReceiveEvent(eventId_t eventid,
                                   MutableBufferVector_ptr datavector_) :
            _eventid(eventid), datavector(datavector_)
        {
        }

        ReceiveEvent::~ReceiveEvent()
        {
        }

        ReceiveEvent::eventId_t ReceiveEvent::eventid() const
        {
            return _eventid;
        }

        MutableBuffer_ptr ReceiveEvent::first()
        {
            return datavector->at(0);
        }
        MutableBuffer_ptr ReceiveEvent::popBack()
        {
            MutableBuffer_ptr last = datavector->back();
            datavector->pop_back();
            return last;
        }

        std::ostream& operator<<(std::ostream& out, const ReceiveEvent& event)
        {
            out << "EventID:" << event.eventid() << ". Data:" << std::endl;
            for(int i = 0; i < event.datavector->size(); i++)
            {
                sevent::socket::MutableBuffer_ptr b = event.datavector->at(i);
                out << "- " << i << " (" << b->size() << "Byte): ";
                out << std::showbase;
                if(b->size() == 4)
                {
                    int* x = b->data<int>();
                    out << std::dec << *x << "|"
                        << std::hex << *x << "|";
                }
                if(b->size() == 2)
                {
                    short* x = b->data<short>();
                    out << std::dec << *x << "|"
                        << std::hex << *x << "|";
                }
                if(b->size() < 40)
                    out.write(b->data<char>(), b->size());
                else
                    out << "<more than 40Bytes is not shown>";
                out << std::endl;
            }
            return out;
        }

    } // namespace socket
} // namespace sevent
