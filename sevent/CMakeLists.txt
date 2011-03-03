include_directories(
	${Boost_INCLUDE_DIRS}
	${sevent_SOURCE_DIR})
link_directories(${Boost_LIBRARY_DIRS})


add_library(sevent STATIC
    EventHandlerMap.cc EventHandlerMap.h

    socket/Service.h
    socket/AsioService.cc socket/AsioService.h
    socket/Address.h socket/Address.cc
    socket/Listener.h
    socket/AsioListener.cc socket/AsioListener.h
    socket/Connector.h
    socket/AsioConnector.cc socket/AsioConnector.h
    socket/Session.cc socket/Session.h
    socket/AsioSession.cc socket/AsioSession.h
    socket/SessionRegistry.cc socket/SessionRegistry.h
    socket/ReceiveEvent.cc socket/ReceiveEvent.h
    socket/Facade.h socket/Facade.cc
    socket/AsioFacade.cc socket/AsioFacade.h
)
target_link_libraries(sevent ${Boost_LIBRARIES})
set_target_properties(sevent PROPERTIES COMPILE_FLAGS -fPIC)
