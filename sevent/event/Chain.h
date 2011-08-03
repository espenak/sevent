#pragma once
#include <sstream>
#include <string>
#include <boost/make_shared.hpp>
#include "HandlerMap.h"


namespace sevent
{
    namespace event
    {
        class Chain;
        typedef boost::shared_ptr<Chain> Chain_ptr;
        class Chain
        {
            public:
                static Chain_ptr make(HandlerMap_ptr handlerMap, const std::string& eventIdPrefix)
                {
                    return boost::make_shared<Chain>(handlerMap, eventIdPrefix);
                }
            public:
                Chain(HandlerMap_ptr handlerMap, const std::string& eventIdPrefix) :
                    _handlerMap(handlerMap), _eventIdPrefix(eventIdPrefix), _counter(0) {}

                void add(HandlerMap::handler_t handler)
                {
                    std::stringstream strstream;
                    strstream << _eventIdPrefix << _counter;
                    std::string eventId = strstream.str();
                    _handlerMap->addEventHandler(eventId, handler);
                    _counter ++;
                }

            private:
                HandlerMap_ptr _handlerMap;
                std::string _eventIdPrefix;
                unsigned _counter;
        };
    } // namespace event
} // namespace sevent
