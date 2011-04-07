include_directories(
	${Boost_INCLUDE_DIRS}
	${sevent_SOURCE_DIR})
link_directories(${Boost_LIBRARY_DIRS})


add_library(sevent STATIC
    EventHandlerMap.cc EventHandlerMap.h

    socket/Address.h socket/Address.cc
    socket/SessionRegistry.cc socket/SessionRegistry.h

    socket/Service.h
    socket/Listener.h
    socket/Connector.h
    socket/Session.cc socket/Session.h
    socket/Facade.h socket/Facade.cc

    asiosocket/AsioFacade.cc asiosocket/AsioFacade.h
    asiosocket/AsioSession.cc asiosocket/AsioSession.h
    asiosocket/AsioService.cc asiosocket/AsioService.h
    asiosocket/AsioConnector.cc asiosocket/AsioConnector.h
    asiosocket/AsioListener.cc asiosocket/AsioListener.h
)
target_link_libraries(sevent ${Boost_LIBRARIES})
set_target_properties(sevent PROPERTIES COMPILE_FLAGS -fPIC)
