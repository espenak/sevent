include_directories(
	${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_library(socketevent STATIC
    SocketService.h
    AsioSocketService.cc AsioSocketService.h
    Address.h
    SocketListener.h
    AsioSocketListener.cc AsioSocketListener.h
    SocketConnector.h
    AsioSocketConnector.cc AsioSocketConnector.h
    SocketSession.cc SocketSession.h
    AsioSocketSession.cc AsioSocketSession.h
    LocalSession.cc LocalSession.h
    SocketSessionRegistry.cc SocketSessionRegistry.h
    
    EventHandlerCollection.cc EventHandlerCollection.h
    EventSocketSessionMediator.cc EventSocketSessionMediator.h
)
target_link_libraries(socketevent ${Boost_LIBRARIES})
