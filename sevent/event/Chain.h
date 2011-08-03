#pragma once
#include <sstream>
#include <string>
#include <boost/make_shared.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
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

                static const std::string nextEventId(Event_ptr event)
                {
                    const std::string& eventid = event->eventid();
                    unsigned prefixOffset = eventid.length() - 3;
                    std::string prefix = eventid.substr(0, prefixOffset);
                    std::string suffix = eventid.substr(prefixOffset, eventid.length());
                    //std::cerr << prefix << std::endl;
                    //std::cerr << suffix << std::endl;
                    unsigned index = boost::lexical_cast<unsigned>(suffix);
                    //std::cerr << index << std::endl;
                    return createEventId(prefix, index + 1);
                }

                static const std::string createEventId(const std::string& prefix,
                                                       unsigned counter)
                {
                    std::stringstream strstream;
                    strstream << prefix
                        << std::setw(3) << std::setfill('0') << counter;
                    return strstream.str();
                }

            public:
                Chain(HandlerMap_ptr handlerMap, const std::string& eventIdPrefix) :
                    _handlerMap(handlerMap), _eventIdPrefix(eventIdPrefix), _counter(0) {}

                void add(HandlerMap::handler_t handler)
                {
                    std::string eventId = createEventId(_eventIdPrefix, _counter);
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
