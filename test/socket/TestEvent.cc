#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEvent
#include <boost/test/unit_test.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <cstring>
#include "sevent/socket/Event.h"
#include "sevent/socket/Buffer.h"
#include <iostream>

using namespace sevent;
//using sevent::StringSerializer;
//typedef Buffer<char, StringSerializer> StringBuffer;



struct CharArray
{
    boost::shared_array<char> array;
    unsigned size;

    CharArray(boost::shared_array<char> array_, unsigned size_) :
        array(array_), size(size_)
    {}

    CharArray(const std::string& str) :
        array(new char[str.size()]), size(str.size())
    {
        memcpy(array.get(), str.c_str(), size);
    }
};
typedef boost::shared_ptr<CharArray> CharArray_ptr;

class CharArraySerialized : public socket::Serialized
{
    private:
        CharArray_ptr _array;
    public:
        CharArraySerialized(CharArray_ptr array) : _array(array) {}
        const char* data() const { return _array->array.get(); }
        uint32_t size() const { return _array->size; }
};

class CharArraySerializer
{
    public:
        static socket::Serialized_ptr serialize(CharArray_ptr array)
        {
            return boost::make_shared<CharArraySerialized>(array);
        }

        static CharArray_ptr deserialize(char* serialized, uint32_t datasize)
        {
            boost::shared_array<char> charArray = boost::shared_array<char>(serialized);
            return boost::make_shared<CharArray>(charArray, datasize);
        }
};

typedef socket::Buffer<CharArray, CharArraySerializer> CharArrayBuffer;





BOOST_AUTO_TEST_CASE( VectorInOut )
{
    CharArray_ptr array = boost::make_shared<CharArray>("Hello World");
    socket::BufferBaseVector_ptr vec = boost::make_shared<socket::BufferBaseVector>();
    vec->push_back(boost::make_shared<CharArrayBuffer>(array));

    boost::shared_ptr<CharArrayBuffer> buf = boost::static_pointer_cast<CharArrayBuffer>(vec->at(0));
    CharArray_ptr out = buf->data();
    BOOST_REQUIRE_EQUAL(out->array.get(), "Hello World");
}

BOOST_AUTO_TEST_CASE( EventNoSerialization )
{
    CharArray_ptr array = boost::make_shared<CharArray>("Hello World");
    socket::BufferBaseVector_ptr vec = boost::make_shared<socket::BufferBaseVector>();
    vec->push_back(boost::make_shared<CharArrayBuffer>(array));

    socket::Event_ptr e = boost::make_shared<socket::Event>(10, vec);
    CharArray_ptr out = e->first<CharArray, CharArraySerializer>();
    BOOST_REQUIRE_EQUAL(out->array.get(), "Hello World");
}




BOOST_AUTO_TEST_CASE( EventSerialization )
{

    CharArray_ptr array = boost::make_shared<CharArray>("Hello World");
    boost::shared_ptr<CharArrayBuffer> buf = boost::make_shared<CharArrayBuffer>(array);
    socket::Serialized_ptr serialized = buf->serialize();
    BOOST_REQUIRE_EQUAL(serialized->data(), "Hello World");

    socket::BufferBaseVector_ptr vec = boost::make_shared<socket::BufferBaseVector>();
    char* serializedData = new char[serialized->size()];
    std::memcpy(serializedData, serialized->data(), serialized->size());
    BOOST_REQUIRE_EQUAL(serializedData, "Hello World");
    vec->push_back(boost::make_shared<socket::BufferBase>(serializedData, serialized->size()));

    socket::Event_ptr e = boost::make_shared<socket::Event>(10, vec);
    CharArray_ptr out = e->first<CharArray, CharArraySerializer>();
    BOOST_REQUIRE_EQUAL(out->array.get(), "Hello World");
}
