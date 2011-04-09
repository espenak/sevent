#pragma once
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/access.hpp>

class Instrumentation
{
    public:
        unsigned id;
        float systemLoad;
    public:
        Instrumentation() {}
        Instrumentation(unsigned id_, float systemLoad_) :
            id(id_), systemLoad(systemLoad_)
        {}


    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ar & id & systemLoad;
        }
};

typedef boost::shared_ptr<Instrumentation> Instrumentation_ptr;
