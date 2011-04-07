#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace sevent
{
    namespace socket
    {
        class BaseSerializeResult
        {
            public:
                virtual const char* data() const = 0;
                virtual unsigned size() const = 0;
        };
        typedef boost::shared_ptr<BaseSerializeResult> BaseSerializeResult_ptr;

        class SerializeResult : public BaseSerializeResult
        {
            public:
                static BaseSerializeResult_ptr make(const char* data, unsigned size)
                {
                    return boost::make_shared<SerializeResult>(data, size);
                }
            public:
                SerializeResult(const char* data, unsigned size) : _data(data), _size(size) {}
                const char* data() const { return _data; }
                unsigned size() const { return _size; }
            private:
                const char* _data;
                unsigned _size;
        };

        class StringSerializeResult: public BaseSerializeResult
        {
            public:
                StringSerializeResult(const std::string& data) : _data(data) {}
                const char* data() const { return _data.c_str(); }
                unsigned size() const { return _data.size(); }
            private:
                std::string _data;

        };
    } // namespace socket
} // namespace sevent
