#pragma once
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>


namespace sevent
{
    namespace socket
    {
        class Serialized
        {
            public:
                virtual const char* data() = 0;
                virtual uint32_t size() = 0;
        };
        typedef boost::shared_ptr<Serialized> Serialized_ptr;

        /** Mutable (changable/non-const) buffer. */
        template<typename T, typename SerializeCls>
        class Buffer
        {
            public:
                Buffer(boost::shared_ptr<T> data) :
                    _data(data) {}
                virtual ~Buffer(){}

                boost::shared_ptr<T> data()
                {
                    return _data;
                }

                const Serialized serialize()
                {
                    return SerializeCls::serialize(_data);
                }

            public:
                static Buffer<T, SerializeCls> deserialize(const char* data, bufsize_t datesize)
                {
                    return SerializeCls::deserialize(data, datesize);
                }

            private:
                boost::shared_ptr<T> _data;
        };

        template<typename T, typename SerializeCls>
        class BufferVector {
            public:
                typedef boost::shared_ptr< Buffer<T, SerializeCls> > Buffer_ptr;
                typedef std::vector<Buffer_ptr> Vector_t;
                typedef boost::shared_ptr<Vector_t> Vector_t_ptr;

            public:
                BufferVector()
                {
                    _vector = boost::make_shared<Vector_t>();
                }

            private:
                Vector_t_ptr _vector;
        };
    } // namespace socket
} // namespace sevent
