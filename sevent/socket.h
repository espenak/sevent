#pragma once
/** \namespace sevent::socket
 *
 * The socket namespace contains an implementation independent interface
 * to all socket communication within the framework.
 *
 * You should simply import sevent/socket.h and use sevent::socket::Facade
 * to work with the socket interface. sevent::socket::Facade::make()
 * creates the currently default socket implementation (a subclass of
 * sevent::socket::Facade).
 *
 * \section Example
 *
 * \include SocketExample.cc
 */


#include "sevent/socket/Facade.h"
