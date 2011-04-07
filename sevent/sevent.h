#pragma once

/**
 * \mainpage
 *
 * \section about About
 *
 * sevent (socket event) is a network-event-handling library. It has a
 * implementation-independent interface(sevent::socket) and a boost::asio
 * implementation (sevent::asiosocket). On top of that it has a couple of
 * utilities making common tasks easier:
 *  - sevent::event
 *  - sevent::boostserialize
 *  - sevent::endiansafe
 * 
 *
 * \section simpleexample Simple example
 *
 * For more examples, see examples/. examples/showcase/ uses most of the
 * features in the library, like endian-safe array-transfer, and
 * boost-serialized object transfer.
 *
 * \subsection SimpleClient SimpleClient.cc
 * \include SimpleClient.cc
 *
 * \subsection SimpleServer SimpleServer.cc
 * \include SimpleServer.cc
 *
 * \subsection SerializablePerson SerializablePerson.h
 * \include SerializablePerson.h
 * */

#include "event.h"
#include "socket.h"
