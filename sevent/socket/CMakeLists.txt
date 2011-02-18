add_library(sevent_socket STATIC
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
    EventData.cc EventData.h
)
target_link_libraries(sevent_socket ${Boost_LIBRARIES})
