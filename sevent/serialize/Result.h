#pragma once
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace sevent
{
    namespace serialize
    {
        class BaseResult
        {
            public:
                virtual const char* data() const = 0;
                virtual unsigned size() const = 0;
        };
        typedef boost::shared_ptr<BaseResult> BaseResult_ptr;


        /** This result only contains a pointer to data stored somewhere
         * else. Usable when serialization results in a pointer to
         * data which does not go out of context before the event is
         * sent. */
        class ConstPtrResult : public BaseResult
        {
            public:
                static BaseResult_ptr make(const char* data, unsigned size)
                {
                    return boost::make_shared<ConstPtrResult>(data, size);
                }
            public:
                ConstPtrResult(const char* data, unsigned size) :
                    _data(data), _size(size) {}
                const char* data() const { return _data; }
                unsigned size() const { return _size; }
            private:
                const char* _data;
                unsigned _size;
        };


        /** String serialization result. If serialization results
         * in a string, this is the appropriate result class. */
        class StringResult: public BaseResult
        {
            public:
                StringResult(const std::string& data) : _data(data) {}
                const char* data() const { return _data.c_str(); }
                unsigned size() const { return _data.size()+1; }
            private:
                std::string _data;

        };
    }
}
