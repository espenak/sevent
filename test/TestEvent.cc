#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/shared_array.hpp>
#include <cstring>
#include <iostream>
#include "sevent/event/Event.h"
#include "sevent/datastruct/SharedArray.h"

using namespace sevent;
using namespace sevent::datastruct;

#ifdef SEVENT_USE_INT_ID
    uint32_t eventid1(1010);
#else
    std::string eventid1("sevent::test::EventOne");
#endif


/////////////////////////////////
// Serializers for testing.
/////////////////////////////////

serialize::BaseResult_ptr serializeUint32SharedArray(boost::any& data)
{
    Uint32SharedArray_ptr a = boost::any_cast<Uint32SharedArray_ptr>(data);
    for(int i = 0; i < a->size; i++)
    {
        a->arr[i] --;
    }
    return serialize::ConstPtrResult::make(reinterpret_cast<const char*>(a->arr.get()),
                                   a->size*sizeof(uint32_t));
}

boost::any deserializeUint32SharedArray(MutableCharArray_ptr serialized)
{
    uint32_t* data = reinterpret_cast<uint32_t*>(serialized->data);
    Uint32SharedArray_ptr arr = boost::make_shared<Uint32SharedArray>(boost::shared_array<uint32_t>(data),
                                                    serialized->size/sizeof(uint32_t));
    return arr;
}
serialize::Pair Uint32TestSerializer(serializeUint32SharedArray, deserializeUint32SharedArray);



typedef boost::shared_ptr<std::string> String_ptr;
serialize::BaseResult_ptr serializeString(boost::any& data)
{
    String_ptr s = boost::any_cast<String_ptr>(data);
    std::stringstream ss;
    ss << "Serialized:" << *s;
    return boost::make_shared<serialize::StringResult>(ss.str());
}

boost::any deserializeString(MutableCharArray_ptr serialized)
{
    String_ptr s = boost::make_shared<std::string>(serialized->data);
    s->insert(0, "De");
    return s;
}
serialize::Pair StringTestSerializer(serializeString,
                                     deserializeString);




struct EventFixture
{
    event::Buffer_ptr inputArray;
    event::Buffer_ptr inputString;

    EventFixture ()
    {
        boost::shared_array<uint32_t> intarr = boost::shared_array<uint32_t>(new uint32_t[2]);
        intarr[0] = 10;
        intarr[1] = 20;
        inputArray = event::Buffer::make(boost::make_shared<Uint32SharedArray>(intarr, 2),
                                         Uint32TestSerializer);

        String_ptr instr = boost::make_shared<std::string>();
        instr->assign("Hello World");
        inputString = event::Buffer::make(instr, StringTestSerializer);
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
    MutableCharArray_ptr received = boost::make_shared<MutableCharArray>(receivedData,
                                                                         serialized->size());

    // Deserialize the "received data"
    event::Buffer_ptr output = event::Buffer::deserialize(received, Uint32TestSerializer);
    Uint32SharedArray_ptr aOut = output->data<Uint32SharedArray_ptr>();
    BOOST_REQUIRE_EQUAL(aOut->arr[0], 9); // The array serializer reduce values by one..
    BOOST_REQUIRE_EQUAL(aOut->arr[1], 19);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}



BOOST_AUTO_TEST_CASE(TestEvent)
{
    event::Event_ptr eventIn = event::Event::make(eventid1, inputArray);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    Uint32SharedArray_ptr aOut = eventIn->first<Uint32SharedArray_ptr>(Uint32TestSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    BOOST_REQUIRE_EQUAL(aOut->arr[0], 10);
    BOOST_REQUIRE_EQUAL(aOut->arr[1], 20);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);
}

BOOST_AUTO_TEST_CASE(TestEventPopBack)
{
    event::Event_ptr event = event::Event::make(eventid1);

    BOOST_REQUIRE_EQUAL(event->size(), 0);
    event->push_back(inputArray);
    BOOST_REQUIRE_EQUAL(event->size(), 1);
    event->pop_back();
    BOOST_REQUIRE_EQUAL(event->size(), 0);

    event->push_back(inputArray);
    event->push_back(inputString);
    BOOST_REQUIRE_EQUAL(event->size(), 2);
    event->pop_back();
    event->pop_back();
    BOOST_REQUIRE_EQUAL(event->size(), 0);


    //Uint32SharedArray_ptr aOut = eventIn->back<Uint32SharedArray_ptr>(Uint32TestSerializer);
    //BOOST_REQUIRE_EQUAL(aOut->arr[0], 10);
}

BOOST_AUTO_TEST_CASE(TestEventBack)
{
    event::Event_ptr event = event::Event::make(eventid1);
    event->push_back(inputArray);
    Uint32SharedArray_ptr aOut = event->back<Uint32SharedArray_ptr>(Uint32TestSerializer);
    BOOST_REQUIRE_EQUAL(aOut->arr[0], 10);

    event->push_back(inputString);
    event->push_back(inputArray);
    aOut = event->back<Uint32SharedArray_ptr>(Uint32TestSerializer);
    BOOST_REQUIRE_EQUAL(aOut->arr[0], 10);
}


MutableCharArray_ptr copyIntoMutArray(serialize::BaseResult_ptr serialized)
{
    char* receivedData = new char[serialized->size()];
    std::memcpy(receivedData, serialized->data(), serialized->size());
    return boost::make_shared<MutableCharArray>(receivedData,
                                                serialized->size());
}


BOOST_AUTO_TEST_CASE(TestEventSerialized)
{
    serialize::BaseResult_ptr serializedArray = inputArray->serialize();
    serialize::BaseResult_ptr serializedString = inputString->serialize();
    BOOST_REQUIRE_EQUAL(serializedString->data(), "Serialized:Hello World");

    // Simulate network transfer (copy the serialized data)
    MutableCharArrayVector_ptr serialized = boost::make_shared<MutableCharArrayVector>();
    serialized->push_back(copyIntoMutArray(serializedArray));
    serialized->push_back(copyIntoMutArray(serializedString));

    event::Event_ptr eventIn = event::Event::make(eventid1, serialized);
    BOOST_REQUIRE_EQUAL(eventIn->size(), 2);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), true);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(1), true);

    Uint32SharedArray_ptr aOut = eventIn->first<Uint32SharedArray_ptr>(Uint32TestSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(0), false);
    BOOST_REQUIRE_EQUAL(aOut->arr[0], 9);  // The array serializer reduce values by one..
    BOOST_REQUIRE_EQUAL(aOut->arr[1], 19);
    BOOST_REQUIRE_EQUAL(aOut->size, 2);

    String_ptr sOut = eventIn->at<String_ptr>(1, StringTestSerializer);
    BOOST_REQUIRE_EQUAL(eventIn->isSerialized(1), false);
    BOOST_REQUIRE_EQUAL(*sOut, "DeSerialized:Hello World");
}


BOOST_AUTO_TEST_SUITE_END()
