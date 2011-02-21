add_library(sevent_serialize STATIC
    Serializable.h
    Serialized.cc Serialized.h
)
target_link_libraries(sevent_serialize ${Boost_LIBRARIES})
