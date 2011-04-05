#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <cstring>
#include <iostream>


struct ConstCharArray;
typedef boost::shared_ptr<ConstCharArray> ConstCharArray_ptr;
struct ConstCharArray
{
    const char* data;
    unsigned size;
    ConstCharArray(const char* data_, unsigned size_) : data(data_), size(size_) {}

    static ConstCharArray_ptr make(const char* data, unsigned size)
    {
        return boost::make_shared<ConstCharArray>(data, size);
    }
};

struct MutableCharArray
{
    char* data;
    unsigned size;
    MutableCharArray(char* data_, unsigned size_) : data(data_), size(size_) {}
};
typedef boost::shared_ptr<MutableCharArray> MutableCharArray_ptr;

typedef boost::function<ConstCharArray_ptr (boost::any& data)> serialize_t;
typedef boost::function<boost::any (MutableCharArray_ptr serialized)> deserialize_t;


class Buffer;
typedef boost::shared_ptr<Buffer> Buffer_ptr;
class Buffer
{
    public:
        static Buffer_ptr make(boost::any anydata, serialize_t serializeFunc)
        {
            return boost::make_shared<Buffer>(anydata, serializeFunc);
        }

        static Buffer_ptr deserialize(MutableCharArray_ptr serialized,
                                      serialize_t serializeFunc,
                                      deserialize_t deserializeFunc)
        {
            boost::any anydata = deserializeFunc(serialized);
            return Buffer::make(anydata, serializeFunc);
        }

    public:
        Buffer(boost::any anydata, serialize_t serializeFunc) :
            _anydata(anydata), _serializeFunc(serializeFunc) {}

        ConstCharArray_ptr serialize()
        {
            return _serializeFunc(_anydata);
        }

        template<typename T> T data()
        {
            return boost::any_cast<T>(_anydata);
        }

    private:
        boost::any _anydata;
        serialize_t _serializeFunc;
};




//typedef std::pair<boost::any, serialize_t> Buffer;
typedef std::vector<Buffer_ptr> BufferVector;


template<typename T>
class Array
{
    public:
        typedef T value_type;
        typedef boost::shared_array<value_type> value_shared_array;
        value_shared_array sharedarr;
        unsigned size;
    public:
        Array(value_shared_array sharedarr_, unsigned size_) :
            sharedarr(sharedarr_), size(size_) {}
};
typedef Array<int> IntArray;
typedef boost::shared_ptr<IntArray> IntArray_ptr;


ConstCharArray_ptr serializeInt(boost::any& data)
{
    IntArray_ptr a = boost::any_cast<IntArray_ptr>(data);
    return ConstCharArray::make(reinterpret_cast<const char*>(a->sharedarr.get()),
                                a->size*sizeof(int));
}

boost::any deserializeInt(MutableCharArray_ptr serialized)
{
    int* data = reinterpret_cast<int*>(serialized->data);
    assert((serialized->size % sizeof(int)) == 0);
    IntArray_ptr arr = boost::make_shared<IntArray>(boost::shared_array<int>(data),
                                                    serialized->size/sizeof(int));
    return arr;
}



class Event;
typedef boost::shared_ptr<Event> Event_ptr;
class Event
{
    private:
        unsigned _eventid;
        BufferVector _buffers;
    public:
        static Event_ptr make(unsigned eventid, Buffer_ptr first)
        {
            return boost::make_shared<Event>(eventid, first);
        }

    public:
        Event(unsigned eventid) :
            _eventid(eventid)
        {}

        Event(unsigned eventid, Buffer_ptr first) :
            _eventid(eventid)
        {
            _buffers.push_back(first);
        }

        template<typename T>
        T first(serialize_t serializeFunc, deserialize_t deserializeFunc)
        {
            return _buffers.at(0)->data<T>();
        }
};


BOOST_AUTO_TEST_CASE(TestLowLevelSerialization)
{
    // Create and serialize an array
    boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
    intarr[0] = 10;
    intarr[1] = 20;
    Buffer_ptr input = Buffer::make(boost::make_shared<IntArray>(intarr, 2),
                                    serializeInt);
    ConstCharArray_ptr serialized = input->serialize();

    // Simulate network transfer (copy the serialized data)
    char* receivedData = new char[serialized->size];
    std::memcpy(receivedData, serialized->data, serialized->size);
    MutableCharArray_ptr received = boost::make_shared<MutableCharArray>(receivedData,
                                                                         serialized->size);

    // Deserialize the "received data"
    Buffer_ptr output = Buffer::deserialize(received, serializeInt, deserializeInt);
    IntArray_ptr aOut = output->data<IntArray_ptr>();
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 10);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 20);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}



BOOST_AUTO_TEST_CASE(TestEvent)
{
    boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
    intarr[0] = 10;
    intarr[1] = 20;
    Buffer_ptr inputArray = Buffer::make(boost::make_shared<IntArray>(intarr, 2),
                                    serializeInt);

    Event_ptr eventIn = Event::make(1010, inputArray);
    IntArray_ptr aOut = eventIn->first<IntArray_ptr>(serializeInt, deserializeInt);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 10);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 20);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}
