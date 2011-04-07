#pragma once
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


        class Result : public BaseResult
        {
            public:
                static BaseResult_ptr make(const char* data, unsigned size)
                {
                    return boost::make_shared<Result>(data, size);
                }
            public:
                Result(const char* data, unsigned size) : _data(data), _size(size) {}
                const char* data() const { return _data; }
                unsigned size() const { return _size; }
            private:
                const char* _data;
                unsigned _size;
        };


        class StringResult: public BaseResult
        {
            public:
                StringResult(const std::string& data) : _data(data) {}
                const char* data() const { return _data.c_str(); }
                unsigned size() const { return _data.size(); }
            private:
                std::string _data;

        };
    } // namespace socket
} // namespace sevent
