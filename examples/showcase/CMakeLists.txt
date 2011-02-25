add_executable(ShowcaseClient Client.cc Shared.h BoostSerializedAddressBook.h)
target_link_libraries(ShowcaseClient sevent)
target_link_libraries(ShowcaseClient ${Boost_LIBRARIES})

add_executable(ShowcaseServer Server.cc Shared.h BoostSerializedAddressBook.h)
target_link_libraries(ShowcaseServer sevent)
target_link_libraries(ShowcaseServer ${Boost_LIBRARIES})
