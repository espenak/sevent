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


struct Serialized
{
    const char* data;
    unsigned size;
    Serialized(const char* data_, unsigned size_) : data(data_), size(size_) {}
};
typedef boost::shared_ptr<Serialized> Serialized_ptr;

typedef boost::function<Serialized_ptr (boost::any& data)> serialize_t;
typedef boost::function<boost::any (Serialized_ptr serialized)> deserialize_t;

typedef std::pair<boost::any, serialize_t> AnyType;
typedef std::vector<AnyType> AnyTypeVector;


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


const Serialized_ptr serializeInt(boost::any& data)
{
    IntArray_ptr a = boost::any_cast<IntArray_ptr>(data);
    return boost::make_shared<Serialized>(reinterpret_cast<const char*>(a->data.get()),
                                          a->size*sizeof(int));
}

AnyType deserializeInt(char* serialized, unsigned size)
{
    int* data = reinterpret_cast<int*>(serialized);
    assert((size%sizeof(int)) == 0);
    IntArray_ptr arr = boost::make_shared<IntArray>(boost::shared_array<int>(data),
                                                    size/sizeof(int));
    return AnyType(arr, serializeInt);
}


BOOST_AUTO_TEST_CASE(VectorInOut)
{
    boost::shared_array<int> intarr = boost::shared_array<int>(new int[2]);
    intarr[0] = 10;
    intarr[1] = 20;
    AnyType a = std::make_pair(boost::make_shared<IntArray>(intarr, 2),
                               serializeInt);
    Serialized_ptr serialized = a.second(a.first);

    // Simulate network transfer (copy the serialized data)
    char* receivedData = new char[serialized->size]; 
    std::memcpy(receivedData, serialized->data, serialized->size);
    AnyType deserialized = deserializeInt(receivedData, serialized->size);
    IntArray_ptr aOut = boost::any_cast<IntArray_ptr>(deserialized.first);
    std::cerr 
        << aOut->data[0] << ":"
        << aOut->data[1] << ":"
        << aOut->size << std::endl;

    //AnyTypeVector vec;
    //std::string s = "Hello world";
    //int i = 10;
    //BOOST_REQUIRE_EQUAL();
}

