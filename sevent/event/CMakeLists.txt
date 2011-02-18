add_library(sevent_event STATIC
    EventHandlerCollection.cc EventHandlerCollection.h
    EventSocketSessionMediator.cc EventSocketSessionMediator.h
)
target_link_libraries(sevent_event ${Boost_LIBRARIES})
target_link_libraries(sevent_socket)