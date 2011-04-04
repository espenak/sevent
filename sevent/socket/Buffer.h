#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
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
                BufferBase() :
                    _serializedData(NULL), _serializedDataSize(0)
                {}

                BufferBase(char* serializedData, uint32_t serializedDataSize) :
                    _serializedData(serializedData), _serializedDataSize(serializedDataSize) {}

                virtual Serialized_ptr serialize()
                {
                    return boost::make_shared<NullSerialized>();
                }
            protected:
                char* _serializedData;
                uint32_t _serializedDataSize;
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
                    _data(data), serializedIsCached(false) {}

                virtual ~Buffer() {}

                boost::shared_ptr<basic_type> data()
                {
                    if(!_data) {
                        //boost::lock_guard<boost::mutex> lock(_freeSerializedDatalock);
                        if(!serializedIsCached) {
                            // TODO: Fix this so a workaround is not needed.
                            // For some reason, using _data here does not work.
                            // As a workaround, we use a separate shared_ptr
                            // to cache data from deserialize.
                            _dataFromDeserialize = SerializeCls::deserialize(_serializedData,
                                                                             _serializedDataSize);
                            serializedIsCached = true;
                        }
                        return _dataFromDeserialize;
                    }
                    return _data;
                }

                Serialized_ptr serialize()
                {
                    return SerializeCls::serialize(_data);
                }

            private:
                boost::shared_ptr<T> _data;
                boost::shared_ptr<T> _dataFromDeserialize;
                boost::mutex _freeSerializedDatalock;
                bool serializedIsCached;
        };

    } // namespace socket
} // namespace sevent
