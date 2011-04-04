#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <cstring>
#include "sevent/StringSerializer.h"
#include "sevent/socket/Event.h"
#include "sevent/socket/Buffer.h"
#include <iostream>

using namespace sevent::socket;
using sevent::StringSerializer;
typedef Buffer<char, StringSerializer> StringBuffer;

//BOOST_AUTO_TEST_CASE( VectorInOut )
//{
    //const char *helloString = "Hello";
    //boost::shared_ptr<char> hello = boost::shared_ptr<char>(new char[std::strlen(helloString)]);
    //std::strcpy(hello.get(), helloString);

    //BufferBaseVector_ptr vec = boost::make_shared<BufferBaseVector>();
    //vec->push_back(Buffer<char, StringSerializer>::make(hello));

    //boost::shared_ptr<StringBuffer> buf = boost::static_pointer_cast<StringBuffer>(vec->at(0));
    //boost::shared_ptr<char> helloOut = buf->data();
    //BOOST_REQUIRE_EQUAL(*helloOut, *hello);
//}

//BOOST_AUTO_TEST_CASE( EventNoSerialization )
//{
    //const char *helloString = "Hello";
    //boost::shared_ptr<char> hello = boost::shared_ptr<char>(new char[std::strlen(helloString)]);
    //std::strcpy(hello.get(), helloString);

    //BufferBaseVector_ptr vec = boost::make_shared<BufferBaseVector>();
    //vec->push_back(Buffer<char, StringSerializer>::make(hello));

    //Event_ptr e = boost::make_shared<Event>(10, vec);
    //boost::shared_ptr<char> helloOut = e->first<char, StringSerializer>();
    //BOOST_REQUIRE_EQUAL(*helloOut, *hello);
//}

BOOST_AUTO_TEST_CASE( EventSerialization )
{
    const char *helloString = "Hello";
    boost::shared_ptr<char> hello = boost::shared_ptr<char>(new char[std::strlen(helloString)]);
    std::strcpy(hello.get(), helloString);
    boost::shared_ptr<StringBuffer> buf = boost::make_shared<StringBuffer>(hello);
    Serialized_ptr serialized = buf->serialize();

    BufferBaseVector_ptr vec = boost::make_shared<BufferBaseVector>();
    char* serializedData = new char[serialized->size()];
    std::memcpy(serializedData, serialized->data(), serialized->size());
    BOOST_REQUIRE_EQUAL(serializedData, "Hello");
    vec->push_back(boost::make_shared<BufferBase>(serializedData, serialized->size()));

    Event_ptr e = boost::make_shared<Event>(10, vec);
    boost::shared_ptr<char> helloOut = e->first<char, StringSerializer>();
    //std::cerr << helloOut << std::endl;
    //BOOST_REQUIRE_EQUAL(*helloOut, *hello);
}
