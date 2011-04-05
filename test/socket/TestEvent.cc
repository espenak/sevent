#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/shared_array.hpp>
#include "sevent/socket/Event.h"
#include <cstring>
#include <iostream>

using namespace sevent::socket;


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


BaseSerializeResult_ptr serializeIntArray(boost::any& data)
{
    IntArray_ptr a = boost::any_cast<IntArray_ptr>(data);
    for(int i = 0; i < a->size; i++)
    {
        a->sharedarr[i] --;
    }
    return SerializeResult::make(reinterpret_cast<const char*>(a->sharedarr.get()),
                                a->size*sizeof(int));
}

boost::any deserializeIntArray(MutableCharArray_ptr serialized)
{
    int* data = reinterpret_cast<int*>(serialized->data);
    IntArray_ptr arr = boost::make_shared<IntArray>(boost::shared_array<int>(data),
                                                    serialized->size/sizeof(int));
    return arr;
}
SerializePair IntSerializer(serializeIntArray, deserializeIntArray);



typedef boost::shared_ptr<std::string> String_ptr;
BaseSerializeResult_ptr serializeString(boost::any& data)
{
    String_ptr s = boost::any_cast<String_ptr>(data);
    std::stringstream ss;
    ss << "Serialized:" << *s;
    return boost::make_shared<StringSerializeResult>(ss.str());
}

boost::any deserializeString(MutableCharArray_ptr serialized)
{
    String_ptr s = boost::make_shared<std::string>(serialized->data);
    s->insert(0, "De");
    return s;
}
SerializePair StringSerializer(serializeString,
                               deserializeString);



struct EventFixture
{
    Buffer_ptr inputArray;
    Buffer_ptr inputString;

    EventFixture ()
    {
        boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
        intarr[0] = 10;
        intarr[1] = 20;
        inputArray = Buffer::make(boost::make_shared<IntArray>(intarr, 2),
                                serializeIntArray);

        String_ptr instr = boost::make_shared<std::string>();
        instr->assign("Hello World");
        inputString = Buffer::make(instr, serializeString);
    }

    ~EventFixture () {}
};

BOOST_FIXTURE_TEST_SUITE(BasicSuite, EventFixture)


BOOST_AUTO_TEST_CASE(TestLowLevelSerialization)
{
    BaseSerializeResult_ptr serialized = inputArray->serialize();

    // Simulate network transfer (copy the serialized data)
    char* receivedData = new char[serialized->size()];
    std::memcpy(receivedData, serialized->data(), serialized->size());
    MutableCharArray_ptr received = boost::make_shared<MutableCharArray>(receivedData,
                                                                         serialized->size());

    // Deserialize the "received data"
    Buffer_ptr output = Buffer::deserialize(received, serializeIntArray, deserializeIntArray);
    IntArray_ptr aOut = output->data<IntArray_ptr>();
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 9); // The array serializer reduce values by one..
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 19);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}



BOOST_AUTO_TEST_CASE(TestEvent)
{
    Event_ptr eventIn = Event::make(1010, inputArray);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    IntArray_ptr aOut = eventIn->first<IntArray_ptr>(IntSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[0], 10);
    BOOST_REQUIRE_EQUAL(aOut->sharedarr[1], 20);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}


MutableCharArray_ptr copyIntoMutArray(BaseSerializeResult_ptr serialized)
{
    char* receivedData = new char[serialized->size()];
    std::memcpy(receivedData, serialized->data(), serialized->size());
    return boost::make_shared<MutableCharArray>(receivedData,
                                                serialized->size());
}


BOOST_AUTO_TEST_CASE(TestEventSerialized)
{
    BaseSerializeResult_ptr serializedArray = inputArray->serialize();
    BaseSerializeResult_ptr serializedString = inputString->serialize();
    BOOST_REQUIRE_EQUAL(serializedString->data(), "Serialized:Hello World");

    // Simulate network transfer (copy the serialized data)
    MutableCharArrayVector_ptr serialized = boost::make_shared<MutableCharArrayVector>();
    serialized->push_back(copyIntoMutArray(serializedArray));
    serialized->push_back(copyIntoMutArray(serializedString));

    Event_ptr eventIn = Event::make(1010, serialized);
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
