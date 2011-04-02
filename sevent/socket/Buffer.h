#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "size.h"


namespace sevent
{
    namespace socket
    {
        class Serialized
        {
            public:
                virtual const char* data() const = 0;
                virtual uint32_t size() const = 0;
        };
        typedef boost::shared_ptr<Serialized> Serialized_ptr;


        class NullSerialized : public Serialized
        {
            public:
                NullSerialized() {}
                const char* data() const { return NULL; }
                uint32_t size() const { return 0; }
        };


        class BufferBase
        {
            public:
                BufferBase() : _serializedData(NULL), _serializedDataSize(0), isSerialized(false) {}
                BufferBase(char* serializedData, uint32_t serializedDataSize) :
                    _serializedData(serializedData), _serializedDataSize(serializedDataSize), isSerialized(true) {}

                virtual Serialized_ptr serialize()
                {
                    return boost::make_shared<NullSerialized>();
                }
            public:
                char* _serializedData;
                uint32_t _serializedDataSize;
                bool isSerialized;
        };
        typedef boost::shared_ptr<BufferBase> BufferBase_ptr;
        typedef std::vector<BufferBase_ptr> BufferBaseVector;
        typedef boost::shared_ptr<BufferBaseVector> BufferBaseVector_ptr;


        /** Mutable (changable/non-const) buffer. */
        template<typename T, typename SerializeCls >
        class Buffer : public BufferBase
        {
                
            public:
                typedef T basic_type;
            public:
                static boost::shared_ptr< Buffer<T, SerializeCls> > make(boost::shared_ptr<T> data)
                {
                    return boost::make_shared< Buffer<T, SerializeCls> >(data);
                }
            public:
                Buffer(boost::shared_ptr<T> data) :
                    _data(data) {}

                virtual ~Buffer()
                {
                    if(isSerialized)
                    {
                        delete[] _serializedData;
                    }
                }

                boost::shared_ptr<basic_type> data()
                {
                    // TODO lock
                    std::cerr << "Is serialized:" << isSerialized << std::endl;
                    if(isSerialized) {
                        _data = SerializeCls::deserialize(_serializedData, _serializedDataSize);
                        delete[] _serializedData;
                        isSerialized = false;
                    }
                    return _data;
                }

                Serialized_ptr serialize()
                {
                    return SerializeCls::serialize(_data);
                }

            private:
                boost::shared_ptr<T> _data;
        };

    } // namespace socket
} // namespace sevent
