#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/shared_array.hpp>
#include <cstring>
#include <iostream>
#include "sevent/event/Event.h"

using namespace sevent;


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



/////////////////////////////////
// Serializers for testing.
/////////////////////////////////

serialize::BaseResult_ptr serializeIntArray(boost::any& data)
{
    IntArray_ptr a = boost::any_cast<IntArray_ptr>(data);
    for(int i = 0; i < a->size; i++)
    {
        a->sharedarr[i] --;
    }
    return serialize::Result::make(reinterpret_cast<const char*>(a->sharedarr.get()),
                                   a->size*sizeof(int));
}

boost::any deserializeIntArray(datastruct::MutableCharArray_ptr serialized)
{
    int* data = reinterpret_cast<int*>(serialized->data);
    IntArray_ptr arr = boost::make_shared<IntArray>(boost::shared_array<int>(data),
                                                    serialized->size/sizeof(int));
    return arr;
}
serialize::Pair IntSerializer(serializeIntArray, deserializeIntArray);



typedef boost::shared_ptr<std::string> String_ptr;
serialize::BaseResult_ptr serializeString(boost::any& data)
{
    String_ptr s = boost::any_cast<String_ptr>(data);
    std::stringstream ss;
    ss << "Serialized:" << *s;
    return boost::make_shared<serialize::StringResult>(ss.str());
}

boost::any deserializeString(datastruct::MutableCharArray_ptr serialized)
{
    String_ptr s = boost::make_shared<std::string>(serialized->data);
    s->insert(0, "De");
    return s;
}
serialize::Pair StringSerializer(serializeString,
                               deserializeString);




struct EventFixture
{
    event::Buffer_ptr inputArray;
    event::Buffer_ptr inputString;

    EventFixture ()
    {
        boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
        intarr[0] = 10;
        intarr[1] = 20;
        inputArray = event::Buffer::make(boost::make_shared<IntArray>(intarr, 2),
                                  IntSerializer);

        String_ptr instr = boost::make_shared<std::string>();
        instr->assign("Hello World");
        inputString = event::Buffer::make(instr, StringSerializer);
    }

    ~EventFixture () {}
};

BOOST_FIXTURE_TEST_SUITE(BasicSuite, EventFixture)


BOOST_AUTO_TEST_CASE(TestLowLevelSerialization)
{
    serialize::BaseResult_ptr serialized = inputArray->serialize();

    // Simulate network transfer (copy the serialized data)
    char* receivedData = new char[serialized->size()];
    std::memcpy(receivedData, serialized->data(), serialized->size());
    datastruct::MutableCharArray_ptr received = boost::make_shared<datastruct::MutableCharArray>(receivedData,
                                                                         serialized->size());

    // Deserialize the "received data"
    event::Buffer_ptr output = event::Buffer::deserialize(received, IntSerializer);
    IntArray_ptr aOut = output->data<IntArray_ptr>();
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 9); // The array serializer reduce values by one..
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 19);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}



BOOST_AUTO_TEST_CASE(TestEvent)
{
    event::Event_ptr eventIn = event::Event::make(1010, inputArray);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    IntArray_ptr aOut = eventIn->first<IntArray_ptr>(IntSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 10);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 20);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}


datastruct::MutableCharArray_ptr copyIntoMutArray(serialize::BaseResult_ptr serialized)
{
    char* receivedData = new char[serialized->size()];
    std::memcpy(receivedData, serialized->data(), serialized->size());
    return boost::make_shared<datastruct::MutableCharArray>(receivedData,
                                                serialized->size());
}


BOOST_AUTO_TEST_CASE(TestEventSerialized)
{
    serialize::BaseResult_ptr serializedArray = inputArray->serialize();
    serialize::BaseResult_ptr serializedString = inputString->serialize();
    BOOST_REQUIRE_EQUAL(serializedString->data(), "Serialized:Hello World");

    // Simulate network transfer (copy the serialized data)
    datastruct::MutableCharArrayVector_ptr serialized = boost::make_shared<datastruct::MutableCharArrayVector>();
    serialized->push_back(copyIntoMutArray(serializedArray));
    serialized->push_back(copyIntoMutArray(serializedString));

    event::Event_ptr eventIn = event::Event::make(1010, serialized);
    BOOST_REQUIRE_EQUAL(eventIn->size(), 2);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), true);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(1), true);

    IntArray_ptr aOut = eventIn->first<IntArray_ptr>(IntSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 9);  // The array serializer reduce values by one..
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 19);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);

    String_ptr sOut = eventIn->at<String_ptr>(1, StringSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(1), false);
    BOOST_REQUIRE_EQUAL(*sOut, "DeSerialized:Hello World");
}


BOOST_AUTO_TEST_SUITE_END()
