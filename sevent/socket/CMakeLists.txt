add_library(sevent_socket STATIC
    Service.h
    AsioService.cc AsioService.h
    Address.h Address.cc
    Listener.h
    AsioListener.cc AsioListener.h
    Connector.h
    AsioConnector.cc AsioConnector.h
    Session.cc Session.h
    AsioSession.cc AsioSession.h
    LocalSession.cc LocalSession.h
    SessionRegistry.cc SessionRegistry.h
    ReceiveEvent.cc ReceiveEvent.h
    SendEvent.cc SendEvent.h
    Facade.h Facade.cc
    AsioFacade.cc AsioFacade.h
)
target_link_libraries(sevent_socket ${Boost_LIBRARIES})
