include_directories(
	${Boost_INCLUDE_DIRS}
	${sevent_SOURCE_DIR})
link_directories(${Boost_LIBRARY_DIRS})


set(sevent_sources
    datastruct/SharedArray.h
    datastruct/MutableCharArray.h

    serialize/Boost.h
    serialize/Pair.h
    serialize/Result.h
    serialize/SharedArray.h
    serialize/String.h

    socket/Address.h socket/Address.cc
    socket/SessionRegistry.cc socket/SessionRegistry.h

    socket/Service.h
    socket/Listener.h
    socket/Connector.h
    socket/Session.cc socket/Session.h
    socket/Facade.cc socket/Facade.h

    asiosocket/AsioFacade.cc asiosocket/AsioFacade.h
    asiosocket/AsioSession.cc asiosocket/AsioSession.h
    asiosocket/AsioService.cc asiosocket/AsioService.h
    asiosocket/AsioConnector.cc asiosocket/AsioConnector.h
    asiosocket/AsioListener.cc asiosocket/AsioListener.h

    event/HandlerMap.cc event/HandlerMap.h
    event/Event.h
    event/Buffer.h
)

add_library(sevent STATIC ${sevent_sources})
target_link_libraries(sevent ${Boost_LIBRARIES})
set_target_properties(sevent PROPERTIES COMPILE_FLAGS -fPIC)
