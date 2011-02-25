add_executable(ShowcaseClient Client.cc Shared.h BoostSerializedAddressBook.h)
target_link_libraries(ShowcaseClient sevent)
target_link_libraries(ShowcaseClient ${Boost_LIBRARIES})

add_executable(ShowcaseServer Server.cc Shared.h BoostSerializedAddressBook.h)
target_link_libraries(ShowcaseServer sevent)
target_link_libraries(ShowcaseServer ${Boost_LIBRARIES})

add_executable(ShowcaseToggleDebugClient ToggleDebugClient.cc Shared.h)
target_link_libraries(ShowcaseToggleDebugClient sevent)
target_link_libraries(ShowcaseToggleDebugClient ${Boost_LIBRARIES})
