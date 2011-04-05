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


struct ConstCharArray
{
    const char* data;
    unsigned size;
    ConstCharArray(const char* data_, unsigned size_) : data(data_), size(size_) {}
};
typedef boost::shared_ptr<ConstCharArray> ConstCharArray_ptr;

struct MutableCharArray
{
    char* data;
    unsigned size;
    MutableCharArray(char* data_, unsigned size_) : data(data_), size(size_) {}
};
typedef boost::shared_ptr<MutableCharArray> MutableCharArray_ptr;

typedef boost::function<ConstCharArray_ptr (boost::any& data)> serialize_t;
typedef boost::function<boost::any (MutableCharArray_ptr serialized)> deserialize_t;


struct Buffer
{
    boost::any data;
    serialize_t serializeFunc;
    Buffer(boost::any& data_, serialize_t serializeFunc_) :
        data(data_), serializeFunc(serializeFunc_) {}
};
typedef boost::shared_ptr<Buffer> Buffer_ptr;




//typedef std::pair<boost::any, serialize_t> Buffer;
typedef std::vector<Buffer_ptr> BufferVector;


template<typename T>
class Array
{
    public:
        typedef T value_type;
        typedef boost::shared_array<value_type> value_shared_array;
        value_shared_array data;
        unsigned size;
    public:
        Array(value_shared_array data_, unsigned size_) :
            data(data_), size(size_) {}
};
typedef Array<int> IntArray;
typedef boost::shared_ptr<IntArray> IntArray_ptr;


ConstCharArray_ptr serializeInt(boost::any& data)
{
    IntArray_ptr a = boost::any_cast<IntArray_ptr>(data);
    return boost::make_shared<ConstCharArray>(reinterpret_cast<const char*>(a->data.get()),
                                          a->size*sizeof(int));
}

boost::any deserializeInt(char* serialized, unsigned size)
{
    int* data = reinterpret_cast<int*>(serialized);
    assert((size%sizeof(int)) == 0);
    IntArray_ptr arr = boost::make_shared<IntArray>(boost::shared_array<int>(data),
                                                    size/sizeof(int));
    return arr;
}


BOOST_AUTO_TEST_CASE(VectorInOut)
{
    boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
    intarr[0] = 10;
    intarr[1] = 20;
    boost::any data = boost::make_shared<IntArray>(intarr, 2);
    Buffer_ptr a = boost::shared_ptr<Buffer>(new Buffer(data,
                                                        serializeInt));
    ConstCharArray_ptr serialized = a->serializeFunc(a->data);

    // Simulate network transfer (copy the serialized data)
    char* receivedData = new char[serialized->size];
    std::memcpy(receivedData, serialized->data, serialized->size);

    // Deserialize the "received data"
    boost::any deserialized = deserializeInt(receivedData, serialized->size);
    IntArray_ptr aOut = boost::any_cast<IntArray_ptr>(deserialized);
    std::cerr 
        << aOut->data[0] << ":"
        << aOut->data[1] << ":"
        << aOut->size << std::endl;

    //BufferVector vec;
    //std::string s = "Hello world";
    //int i = 10;
    //BOOST_REQUIRE_EQUAL();
}

